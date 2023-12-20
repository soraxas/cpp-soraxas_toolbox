/*
 * MIT License
 *
 * Copyright (c) 2019-2023 Tin Yiu Lai (@soraxas)
 *
 * This file is part of the project soraxas_toolbox, a collections of utilities
 * for developing c++ applications.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "print_utils_core.h"

#include <Eigen/Dense>

namespace sxs
{

namespace eigen
{

    void print_eigen_info(const Eigen::MatrixXd &input)
    {
        println("=================");
        println_spaced("Dim (rows,cols):", input.rows(), input.cols());
        println(input);
        println("=================");
    }

    /**
     * def extendMatrixRowBoundary(arr, n):
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
    void extendMatrixRowBoundary(
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

        extendMatrixRowBoundary(input, n_neighbours, output);

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

    /**
     * This function takes in a set of points, a target segment distance, and optional flags for
     * including the last point and intermediate points.
     *
     * The return value is a list of points what had "walked" equal distance along the path.
     * Note that it DOES NOT guarentee the distance between each consecutive points have the
     * specified distance (and in face, it will always be less than or equal to the target segment
     * distance). However, each consecutive points had ineed "walked" the specified distance along
     * the given path
     *
     * See:
     * https://github.com/soraxas/plotting-gallery/blob/51c7cc7cb3d753b1f4988609499082f91ba56dfa/plots/Subdivide_points_with_equal_distance.ipynb
     *
     * Essentially, we:
     * 1. Calculates the distance along the segment between consecutive points
     * 2. Enters a while-loop to subdivide the current segment into smaller segments if its length
     * exceeds the target_segdist.
     * 3. Computes the remaining distance needed to reach the target_segdist from the current point.
     * 4. Interpolates a new point along the segment based on the remaining distance or solely based
     * on the current segment if no distance is left over from previous segments.
     * 5. Stores the interpolated point in the output vector
     *
     */
    template <typename DataType, int Rows>
    auto subdivide_line_segments_equal_distance(
        const std::vector<Eigen::Matrix<DataType, Rows, 1>> &points, DataType target_segdist,
        bool include_last_pt = false, bool include_intermediate_pt = false
    )
    {
        using VectorType = Eigen::Matrix<DataType, Rows, 1>;

        if (points.size() < 1)
            return std::vector<VectorType>{};

        std::vector<VectorType> out;
        out.push_back(points[0]);

        DataType dist_leftover = 0.0;
        for (int i = 1; i < points.size(); ++i)
        {
            DataType dist_along_segment = (points[i] - points[i - 1]).norm();

            VectorType diff_vec = points[i] - points[i - 1];
            VectorType diff_unit_vec = diff_vec.normalized();

            // Check if adding the current segment exceeds the required distance for sampling
            while (dist_leftover + dist_along_segment >= target_segdist)
            {
                DataType remaining_distance = target_segdist - dist_leftover;

                VectorType interpolated_point;
                if (dist_leftover > 0)
                {
                    // left-over from last segment
                    double ratio = remaining_distance / dist_along_segment;
                    interpolated_point = points[i - 1] + ratio * diff_vec;
                }
                else
                {
                    // purely from the current segment
                    interpolated_point = out.back() + diff_unit_vec * target_segdist;
                }

                // reset
                dist_leftover = 0.0;
                out.push_back(interpolated_point);
                // walk along this segment
                dist_along_segment -= remaining_distance;
            }

            // left over from this segment.
            dist_leftover += dist_along_segment;

            if (include_intermediate_pt && dist_leftover > 0)
            {
                // snap to the end of this segment
                out.push_back(points[i]);
                dist_leftover = 0;
            }
        }

        if (include_last_pt && dist_leftover > 0)
        {
            // snap to the end of whole path
            out.push_back(points[points.size() - 1]);
        }

        return out;
    }

}  // namespace eigen
}  // namespace sxs

#ifdef SXS_RUN_TESTS
/*
 * -------------------------------------------
 * Test cases and general usage for this file:
 * -------------------------------------------
 */

#include "eigen_helpers.h"

template <typename Derived>
void check_vector_of_eigen_vectors_are_equal(
    const std::vector<Derived> &expected, const std::vector<Derived> &reality
)
{
    CHECK_EQ(expected.size(), reality.size());
    for (size_t i = 0; i < std::min(expected.size(), reality.size()); ++i)
    {
        for (size_t j = 0; j < expected[i].size(); ++j)
        {
            CHECK(expected[i][j] == doctest::Approx(reality[i][j]));
        }
    }
}

TEST_CASE("[sxs eigen_math] check expanding boundary")
{
    Eigen::MatrixXd input(5, 3);

    input << 1, 1, 1,  //
        2, 2, 2,       //
        3, 3, 3,       //
        4, 4, 4,       //
        5, 5, 5;

    SUBCASE("")
    {
        Eigen::MatrixXd expected_output(7, 3);
        expected_output << 1, 1, 1,  //
            1, 1, 1,                 //
            2, 2, 2,                 //
            3, 3, 3,                 //
            4, 4, 4,                 //
            5, 5, 5,                 //
            5, 5, 5;

        Eigen::MatrixXd output;
        sxs::eigen::extendMatrixRowBoundary(input, 1, output);
        CHECK_EQ(output.rows(), expected_output.rows());
        CHECK_EQ(output.cols(), expected_output.cols());
        CHECK_EQ(output, expected_output);
    }
}

TEST_CASE("[sxs eigen_math] check cumsum")
{
    using namespace sxs::eigen;
    Eigen::MatrixXd input(5, 3);
    Eigen::MatrixXd output;
    Eigen::MatrixXd expected_output;

    input << 1, 1, 1,  //
        2, 2, 2,       //
        3, 3, 3,       //
        4, 4, 4,       //
        5, 5, 5;

    SUBCASE("")
    {
        expected_output.resize(5, 3);
        expected_output << 1, 1, 1,  //
            3, 3, 3,                 //
            6, 6, 6,                 //
            10, 10, 10,              //
            15, 15, 15;
        cumsum(input, EigenOpsWise::rowwise, false, output);
    }

    SUBCASE("")
    {
        expected_output.resize(6, 3);
        expected_output << 0, 0, 0,  //
            1, 1, 1,                 //
            3, 3, 3,                 //
            6, 6, 6,                 //
            10, 10, 10,              //
            15, 15, 15;
        cumsum(input, EigenOpsWise::rowwise, true, output);
    }

    SUBCASE("")
    {
        expected_output.resize(5, 3);
        expected_output << 1, 2, 3,  //
            2, 4, 6,                 //
            3, 6, 9,                 //
            4, 8, 12,                //
            5, 10, 15;               //
        cumsum(input, EigenOpsWise::colwise, false, output);
    }

    SUBCASE("")
    {
        expected_output.resize(5, 4);
        expected_output << 0, 1, 2, 3,  //
            0, 2, 4, 6,                 //
            0, 3, 6, 9,                 //
            0, 4, 8, 12,                //
            0, 5, 10, 15;               //

        cumsum(input, EigenOpsWise::colwise, true, output);
    }
    CHECK_EQ(output.rows(), expected_output.rows());
    CHECK_EQ(output.cols(), expected_output.cols());
    CHECK_EQ(output, expected_output);
}

TEST_CASE("[sxs eigen_math] check subdivide_line_segments_equal_distance")
{
    using namespace sxs::eigen;

    SUBCASE("point list 1")
    {
        std::vector<Eigen::Vector2d> input = {
            {0., 0.},    //
            {1., 0.},    //
            {5., 5.},    //
            {5., 8.},    //
            {5.5, 10.},  //
            {10., 0.}};

        SUBCASE("1")
        {
            std::vector<Eigen::Vector2d> expected = {
                {0., 0.},                  //
                {3.49878019, 3.12347524},  //
                {5., 7.59687576},          //
                {6.54040698, 7.68798448},  //
                {8.59223037, 3.1283769}};

            check_vector_of_eigen_vectors_are_equal(
                expected, sxs::eigen::subdivide_line_segments_equal_distance(input, 5.)
            );
        }

        SUBCASE("2")
        {
            std::vector<Eigen::Vector2d> expected = {
                {0., 0.},                  //
                {1.37481703, 0.46852129},  //
                {2.3743291, 1.71791138},   //
                {3.37384118, 2.96730148},  //
                {4.37335326, 4.21669157},  //
                {5., 5.59687576},          //
                {5., 7.19687576},          //
                {5.19327076, 8.77308304},  //
                {5.63760469, 9.69421179},  //
                {6.29418818, 8.23513738},  //
                {6.95077166, 6.77606297},  //
                {7.60735515, 5.31698857},  //
                {8.26393863, 3.85791416},  //
                {8.92052211, 2.39883975},  //
                {9.5771056, 0.9397653}};
            check_vector_of_eigen_vectors_are_equal(
                expected, sxs::eigen::subdivide_line_segments_equal_distance(input, 1.6)
            );
        }

        SUBCASE("3")
        {
            std::vector<Eigen::Vector2d> expected = {
                {0., 0.},                  //
                {1., 0.},                  //
                {1.99951208, 1.2493901},   //
                {2.99902415, 2.49878019},  //
                {3.99853623, 3.74817029},  //
                {4.9980483, 4.99756038},   //
                {5., 5.},                  //
                {5., 6.6},                 //
                {5., 8.},                  //
                {5.388057, 9.552228},      //
                {5.5, 10.},                //
                {6.15658348, 8.54092559},  //
                {6.81316697, 7.08185118},  //
                {7.46975045, 5.62277678},  //
                {8.12633393, 4.16370237},  //
                {8.78291742, 2.70462796},  //
                {9.4395009, 1.24555355},   //
                {10., 0.}};
            check_vector_of_eigen_vectors_are_equal(
                expected,
                sxs::eigen::subdivide_line_segments_equal_distance(input, 1.6, false, true)
            );
        }
    }

    SUBCASE("point list 2")
    {
        std::vector<Eigen::Vector2d> input = {
            {0., 0.},   //
            {1., 0.},   //
            {1., 4.5},  //
            {1.51, 4.5}};

        SUBCASE("1")
        {
            std::vector<Eigen::Vector2d> expected = {
                {0., 0.},    //
                {0.75, 0.},  //
                {1., 0.5},   //
                {1., 1.25},  //
                {1., 2.},    //
                {1., 2.75},  //
                {1., 3.5},   //
                {1., 4.25},  //
                {1.5, 4.5},  //
                {1.51, 4.5}};
            check_vector_of_eigen_vectors_are_equal(
                expected, sxs::eigen::subdivide_line_segments_equal_distance(input, .75, true)
            );
        }

        SUBCASE("2")
        {
            std::vector<Eigen::Vector2d> expected = {
                {0., 0.},     //
                {0.73, 0.},   //
                {1., 0.46},   //
                {1., 1.19},   //
                {1., 1.92},   //
                {1., 2.65},   //
                {1., 3.38},   //
                {1., 4.11},   //
                {1.34, 4.5},  //
                {1.51, 4.5}};
            check_vector_of_eigen_vectors_are_equal(
                expected, sxs::eigen::subdivide_line_segments_equal_distance(input, .73, true)
            );
        }

        SUBCASE("3")
        {
            std::vector<Eigen::Vector2d> expected = {
                {0., 0.},    //
                {0.73, 0.},  //
                {1., 0.46},  //
                {1., 1.19},  //
                {1., 1.92},  //
                {1., 2.65},  //
                {1., 3.38},  //
                {1., 4.11},  //
                {1.34, 4.5}};
            SUBCASE("test using MatrixXd as input")
            {
                Eigen::MatrixXd input_(4, 2);
                input_ << 0., 0.,  //
                    1., 0.,        //
                    1., 4.5,       //
                    1.51, 4.5;
                input = map_eigen_mat_to_std_vector<2>(input_);
            }
            SUBCASE("normal case")
            {
            }
            check_vector_of_eigen_vectors_are_equal(
                expected, sxs::eigen::subdivide_line_segments_equal_distance(input, .73)
            );
        }
    }
}

#endif  // SXS_RUN_TESTS
