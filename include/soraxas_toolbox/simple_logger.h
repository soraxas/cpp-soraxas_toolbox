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

#include <iostream>
#include <string>

namespace simple_logger
{

enum log_level_t
{
    LOG_NOTHING,
    LOG_CRITICAL,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG
};

struct GlobalData
{
    log_level_t GLOBAL_LEVEL = LOG_NOTHING;
    std::string prefix = "";
};

inline GlobalData &get_global_data()
{
    static GlobalData data;
    return data;
}

inline void set_verbosity(log_level_t level = LOG_DEBUG)
{
    get_global_data().GLOBAL_LEVEL = level;
}

inline void set_silence()
{
    get_global_data().GLOBAL_LEVEL = LOG_NOTHING;
}

class DummyStreamBuf : public std::streambuf
{
};

inline std::ostream &log(log_level_t thereshold)
{
    static DummyStreamBuf dummystreambuf;
    static std::ostream dummystream(&dummystreambuf);
    std::ostream &stream = (get_global_data().GLOBAL_LEVEL >= thereshold) ? std::cerr : dummystream;

    stream << get_global_data().prefix;

    return stream;
}
}  // namespace simple_logger