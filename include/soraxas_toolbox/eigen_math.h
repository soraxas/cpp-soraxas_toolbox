#pragma once

#include <Eigen/Dense>

using namespace sxs;

void print_eigen_info(const Eigen::MatrixXd &input)
{
    println("=================");
    println_spaced("Dim (rows,cols):", input.rows(), input.cols());
    println(input);
    println("=================");
}

/**
 * def extendMatrixBoundary(arr, n):
    new_arr = np.empty((arr.shape[0]+2*n, arr.shape[1]))
    _size = arr.shape[0]
    new_arr[0:n, :] = arr[0, :]

    new_arr[n:n+_size, :] = arr[:, :]

    new_arr[_size:_size+n, :] = arr[_size-1, :]
 *
*/
/** E.g.
 * Given (input, 2):
 *                                 1 1 1
 *                                 1 1 1
 *          1 1 1                  1 1 1
 *          2 2 2                  2 2 2
 *  input = 3 3 3  ; then output = 3 3 3
 *          4 4 4                  4 4 4
 *          5 5 5                  5 5 5
 *                                 5 5 5
 *                                 5 5 5
 */
template <typename DerivedX, typename DerivedY>
void extendMatrixBoundary(
    const Eigen::MatrixBase<DerivedX> &input, size_t n_extend,
    Eigen::PlainObjectBase<DerivedY> &output
)
{
    using Eigen::all;
    using Eigen::last;
    using Eigen::seq;

    const auto n_elements = input.rows();

    output.resize(input.rows() + 2 * n_extend, input.cols());

    // assign the beginning extended bits
    output(seq(0, n_extend - 1), all).rowwise() = input(0, all);

    // assign the ending extended bits
    output(seq(n_extend + n_elements, last), all).rowwise() = input(last, all);

    // assign the middle bits with the actual elements
    output(seq(n_extend, n_extend + n_elements - 1), all) = input;
}

enum class EigenOpsWise
{
    rowwise,
    colwise,
};

/** E.g.
 * Given cumsum(input, EigenOpsWise::rowwise, false, cumsum_output);
 *          1 1 1                   1  1  1
 *          2 2 2                   3  3  3
 *  input = 3 3 3  ; then output =  6  6  6
 *          4 4 4                  10 10 10
 *          5 5 5                  15 15 15
 *
 * Given cumsum(input, EigenOpsWise::colwise, true, cumsum_output);
 *          1 1 1                  0  1  2  3
 *          2 2 2                  0  2  4  6
 *  input = 3 3 3  ; then output = 0  3  6  9
 *          4 4 4                  0  4  8 12
 *          5 5 5                  0  5 10 15
 */
template <typename DerivedX, typename DerivedY>
void cumsum(
    const Eigen::MatrixBase<DerivedX> &X, EigenOpsWise opt_wise, const bool zero_prefix,
    Eigen::PlainObjectBase<DerivedY> &Y
)
{
    using namespace Eigen;
    using namespace std;

    Eigen::Index num_outer, num_inner;
    Eigen::Index extra_row_num = 0;
    Eigen::Index extra_col_num = 0;

    switch (opt_wise)
    {
        case EigenOpsWise::rowwise:
            num_outer = X.cols();
            num_inner = X.rows();
            break;
        case EigenOpsWise::colwise:
            num_outer = X.rows();
            num_inner = X.cols();
            break;
    }

    if (zero_prefix)
    {
        switch (opt_wise)
        {
            case EigenOpsWise::rowwise:
                extra_row_num = 1;
                break;
            case EigenOpsWise::colwise:
                extra_col_num = 1;
                break;
        }
    }

    Y.resize(X.rows() + extra_row_num, X.cols() + extra_col_num);
    // get number of columns (or rows)

    // This has been optimized so that dim = 1 or 2 is roughly the same cost.
    // (Optimizations assume ColMajor order)
    if (opt_wise == EigenOpsWise::rowwise)
    {
        if (zero_prefix)
        {
            Y.row(0).setConstant(0);
        }
#pragma omp parallel for
        for (Eigen::Index o = 0; o < num_outer; o++)
        {
            typename DerivedX::Scalar sum = 0;
            for (Eigen::Index i = 0; i < num_inner; i++)
            {
                sum += X(i, o);
                const Eigen::Index yi = zero_prefix ? i + 1 : i;
                Y(yi, o) = sum;
            }
        }
    }
    else
    {
        if (zero_prefix)
        {
            Y.col(0).setConstant(0);
        }
        for (Eigen::Index i = 0; i < num_inner; i++)
        {
            const Eigen::Index yi = zero_prefix ? i + 1 : i;
            // Notice that it is *not* OK to put this above the inner loop
            // Though here it doesn't seem to pay off...
            // #pragma omp parallel for
            for (Eigen::Index o = 0; o < num_outer; o++)
            {
                if (i == 0)
                {
                    Y(o, yi) = X(o, i);
                }
                else
                {
                    Y(o, yi) = Y(o, yi - 1) + X(o, i);
                }
            }
        }
    }
}

/**
    out = ((cumsum[2*n + 1:, :] - cumsum[:_size, :]) / (2*n+1))
*/
/** E.g.
 * Given cumsum(input, EigenOpsWise::rowwise, false, cumsum_output);
 *          1 1 1                   1  1  1
 *          2 2 2                   3  3  3
 *  input = 3 3 3  ; then output =  6  6  6
 *          4 4 4                  10 10 10
 *          5 5 5                  15 15 15
 *
 * Given cumsum(input, EigenOpsWise::colwise, true, cumsum_output);
 *          1 1 1                  0  1  2  3
 *          2 2 2                  0  2  4  6
 *  input = 3 3 3  ; then output = 0  3  6  9
 *          4 4 4                  0  4  8 12
 *          5 5 5                  0  5 10 15
 */
template <typename DerivedX, typename DerivedY>
void moving_average(
    const Eigen::MatrixBase<DerivedX> &input, size_t n_neighbours,
    Eigen::PlainObjectBase<DerivedY> &output
)
{
    using Eigen::all;
    using Eigen::last;
    using Eigen::seq;

    const auto n_elements = input.rows();

    extendMatrixBoundary(input, n_neighbours, output);

    static thread_local Eigen::MatrixXd cumsum_output;
    cumsum(output, EigenOpsWise::rowwise, true, cumsum_output);

    output = (cumsum_output(seq(2 * n_neighbours + 1, last), all) -
              cumsum_output(seq(0, n_elements - 1), all)) /
             (2 * n_neighbours + 1);
}

template <EigenOpsWise opt_wise = EigenOpsWise::rowwise, typename DerivedX>
auto normalise_vector(const Eigen::MatrixBase<DerivedX> &input)
{
    if constexpr (opt_wise == EigenOpsWise::rowwise)
        return input.array().colwise() / input.rowwise().norm().array();
    else
        return input.array().rowwise() / input.colwise().norm().array();
}
