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

#ifndef SXS_CEREAL_CONSTRUCT_OR_LOAD_H
#define SXS_CEREAL_CONSTRUCT_OR_LOAD_H

#include "soraxas_toolbox/main.h"
#include <cereal/archives/binary.hpp>

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

namespace sxs
{

    template <typename T, typename... Args>
    std::shared_ptr<T> construct_or_load(const std::string &cache_fname, Args &&...constructor_args)
    {
        static_assert(
            sizeof...(constructor_args) >= 1, "Constructing object with zero argunment "
                                              "is explicitly "
                                              "disallowed, as "
                                              "it is used by cereal for "
                                              "deserialisation."
        );

        std::shared_ptr<T> target_obj;

        if (
#if SXS_CONSTRUCT_OR_LOAD_FORCE_NO_CACHE
            false &&
#endif
            std::filesystem::exists(cache_fname))
        {
            println("Using cache file ", cache_fname);
            std::ifstream ss(cache_fname);            // any stream can be used
            cereal::BinaryInputArchive iarchive(ss);  // Create an input archive
            iarchive(target_obj);                     // Read the data from the archive
        }
        else
        {
            target_obj = std::make_shared<T>(std::forward<Args>(constructor_args)...);

            std::ofstream ss(cache_fname);  // any stream can be used

            cereal::BinaryOutputArchive oarchive(ss);  // Create an output archive
            oarchive(target_obj);                      // Write the data to the archive
                                                       // archive goes out of scope, ensuring all
                                                       // contents are flushed
        }
        return target_obj;
    }
}  // namespace sxs

#endif  // SXS_CEREAL_CONSTRUCT_OR_LOAD_H
