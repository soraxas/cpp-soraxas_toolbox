
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

#define SXS_RUN_TESTS

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include "doctest.h"

#include <soraxas_toolbox/clock.h>
#include <soraxas_toolbox/compile_time_dict.h>
#include <soraxas_toolbox/format.h>
#include <soraxas_toolbox/globals.h>
#include <soraxas_toolbox/metaprogramming.h>
#include <soraxas_toolbox/print_utils.h>
#include <soraxas_toolbox/stats/timer.h>
#include <soraxas_toolbox/stats/token.h>
#include <soraxas_toolbox/vector_math.h>

#ifdef HAS_EIGEN_
#include <soraxas_toolbox/eigen_helpers.h>
#include <soraxas_toolbox/eigen_math.h>
#endif