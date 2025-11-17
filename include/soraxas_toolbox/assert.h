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

// https://stackoverflow.com/questions/3767869/adding-message-to-assert
#ifndef ASSERT
#ifdef NDEBUG
#define ASSERT(...) 0
#else
// https://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros
//  The multiple macros that you would need anyway [as per: Crazy Eddie]
#define ASSERT_0() 0
#define ASSERT_1(condition) ASSERT_2(condition, "")
#define ASSERT_2(condition, message)                                                               \
    (!(condition)) ? (std::cerr << "Assertion failed: (" << #condition << "), "                    \
                                << "function " << __FUNCTION__ << ", file " << __FILE__            \
                                << ", line " << __LINE__ << "." << std::endl                       \
                                << message << std::endl,                                           \
                      abort(), 0) :                                                                \
                     1
// The interim macro that simply strips the excess and ends up with the required
// macro
#define ASSERT_X(x, A, B, FUNC, ...) FUNC

// The macro that the programmer uses
#define ASSERT(...)                                                                                \
    ASSERT_X(, ##__VA_ARGS__, ASSERT_2(__VA_ARGS__), ASSERT_1(__VA_ARGS__), ASSERT_0(__VA_ARGS__))
#endif
#endif
