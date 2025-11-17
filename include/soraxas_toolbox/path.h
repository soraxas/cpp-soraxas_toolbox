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

//
// Created by tin on 11/08/22.
//

#ifndef SXS_PATH_UTILS_H
#define SXS_PATH_UTILS_H

#include "future.h"

#include <pwd.h>     // for getting home dir
#include <unistd.h>  // for getting home dir

namespace sxs
{
inline std::string get_home_dir()
{
    /*
     * Return the current home directory
     */
    const char *homedir;

    // Check $HOME environment variable first before retrieving user's
    // homedir
    if ((homedir = getenv("HOME")) == NULL)
    {
        homedir = getpwuid(getuid())->pw_dir;
    }

    return homedir;
}
}  // namespace sxs

#endif  // SXS_PATH_UTILS_H
