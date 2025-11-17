/*
 * MIT License
 *
 * Copyright (c) 2019-2025 Tin Yiu Lai (@soraxas)
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
#include <Eigen/Dense>
#include <cereal/access.hpp>
#include <cereal/cereal.hpp>

#include <fstream>

// container and such
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>

namespace cereal
{

template <
    class Archive, class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
inline void
save(Archive &ar, const ::Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> &m)
{
    int rows = m.rows(), cols = m.cols();
    ar(CEREAL_NVP(rows), CEREAL_NVP(cols));
    const _Scalar *data = m.data();
    for (uint32_t i = 0; i < m.size(); ++i)
    {
        ar(data[i]);
    }
}

template <
    class Archive, class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
inline void
load(Archive &ar, ::Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> &m)
{
    int rows, cols;
    ar(CEREAL_NVP(rows), CEREAL_NVP(cols));
    if (rows * cols != m.size())
        m.resize(rows, cols);
    _Scalar value;
    _Scalar *data = m.data();
    for (uint32_t i = 0; i < m.size(); ++i)
    {
        ar(value);
        data[i] = value;
    }
}

template <class Archive, class _Scalar, int _AmbientDim>
inline void serialize(Archive &ar, ::Eigen::AlignedBox<_Scalar, _AmbientDim> &bb)
{
    ar(make_nvp("min", bb.min()), make_nvp("max", bb.max()));
}

template <class Archive, class _Scalar, int _AmbientDim, int _Options>
inline void serialize(Archive &ar, ::Eigen::Hyperplane<_Scalar, _AmbientDim, _Options> &plane)
{
    ar(make_nvp("coeffs", plane.coeffs()));
}

template <class Archive, class _Scalar, int _Options>
inline void serialize(Archive &ar, ::Eigen::Quaternion<_Scalar, _Options> &quat)
{
    ar(make_nvp("w", quat.w()), make_nvp("x", quat.x()), make_nvp("y", quat.y()),
       make_nvp("z", quat.z()));
}

}  // namespace cereal
