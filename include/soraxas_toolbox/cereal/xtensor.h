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

//
// Created by tin on 22/07/22.
//

#ifndef SXS_CEREAL_XTENSOR_H
#define SXS_CEREAL_XTENSOR_H

#include "soraxas_toolbox/cereal/serialisable_stream.h"
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <xtensor/xnpy.hpp>

namespace cereal
{

// ==================================================
// xarray
// ==================================================

template <class Archive, typename T>
void save(Archive &archive, const xt::xarray_container<T> &m)
{
    sxs::SerialisableBinaryStream sbs;
    xt::detail::dump_npy_stream(sbs.get_ostream(), m);
    archive(sbs);
}

template <class Archive, typename T>
void load(Archive &archive, xt::xarray_container<T> &m)
{
    sxs::SerialisableBinaryStream sbs;
    archive(sbs);
    m = xt::load_npy<typename T::value_type>(sbs.get_istream());
}

// ==================================================
// xtensor
// ==================================================

template <class Archive, typename T, std::size_t N>
void save(Archive &archive, const xt::xtensor_container<T, N> &m)
{
    sxs::SerialisableBinaryStream sbs;
    xt::detail::dump_npy_stream(sbs.get_ostream(), m);
    archive(sbs);
}

template <class Archive, typename T, std::size_t N>
void load(Archive &archive, xt::xtensor_container<T, N> &m)
{
    sxs::SerialisableBinaryStream sbs;
    archive(sbs);
    m = xt::load_npy<typename T::value_type>(sbs.get_istream());
}

// ==================================================
// xfixed
// ==================================================

template <class Archive, typename T, typename N>
void save(Archive &archive, const xt::xfixed_container<T, N> &m)
{
    sxs::SerialisableBinaryStream sbs;
    xt::detail::dump_npy_stream(sbs.get_ostream(), m);
    archive(sbs);
}

template <class Archive, typename T, typename N>
void load(Archive &archive, xt::xfixed_container<T, N> &m)
{
    sxs::SerialisableBinaryStream sbs;
    archive(sbs);
    m = xt::load_npy<T>(sbs.get_istream());
}

}  // namespace cereal

#endif  // SXS_CEREAL_XTENSOR_H
