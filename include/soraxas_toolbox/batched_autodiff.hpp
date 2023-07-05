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

#include <autodiff/forward.hpp>

namespace sxs
{

    template <
        typename Function, typename Args, typename Result, typename dtype, int MatData_B,
        int MatData_N, int MatData_order>
    auto batched_gradient_single_var(
        const Function &f,
        Eigen::Matrix<autodiff::forward::Dual<dtype, dtype>, MatData_B, MatData_N, MatData_order>
            &batched_var,
        Args &&args, Result &result
    ) -> Eigen::Matrix<dtype, MatData_B, MatData_N, MatData_order>
    {
        // Unlike the vendor function, this only works for one variable
        // This assumes eigen shape [B,N] where B is the number of batch,
        // and N is the vector shape of input.
        //
        // Usage: `batched_var` must be in the shape of [B,N]
        //        `result` must be in the shape of [B,1] or [1,B]
        //        `f` takes input [B,N], and output to result [B,1] or [1,B]
        //
        // Return: `g` which is the gradient with shape [B,N]

        // setup holder for gradient
        Eigen::Matrix<dtype, MatData_B, MatData_N, MatData_order> g(
            batched_var.rows(), batched_var.cols()
        );

        for (auto j = 0; j < batched_var.cols(); ++j)
        {  // for each variable

            // setup grad for forward pass
            for (auto i = 0; i < batched_var.rows(); ++i)
            {  // for each pt
                batched_var(i, j).grad = 1.0;
            }

            // execute the function
            result = std::apply(f, args);

            // collect grad
            for (auto i = 0; i < batched_var.rows(); ++i)
            {  // for each pt
                batched_var(i, j).grad = 0.0;
                g(i, j) = autodiff::val(result(i).grad);
            }
        }

        return g;
    }

}  // namespace sxs
