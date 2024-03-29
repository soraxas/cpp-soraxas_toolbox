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

#include "assert.h"
#include "forward_declare.h"
#include "print_utils.h"
#include "string.h"
#include "vector_math.h"

#include <cmath>     // for std::sqrt
#include <iterator>  // needed for std::ostram_iterator
#include <string>

// #define SXS_USE_PPRINT

////////////////////////////////////////////////////////////////////////
namespace sxs
{
namespace vec
{
    // extend a vector with a vector
    template <typename T>
    inline void extend(std::vector<T> &container, const std::vector<T> &v2)
    {
        // container.reserve(container.size() + distance(v2.begin(),
        // v2.end()));  // more generic
        container.reserve(container.size() + v2.size());
        container.insert(container.end(), v2.begin(), v2.end());
    }

    template <typename T>
    inline T sum(const std::vector<T> &container)
    {
        assert(container.size() > 0);
        T total = container[0];  // should be copy construct
        for (size_t i = 1; i < container.size(); ++i)
            total += container[i];
        return total;
    }

    template <typename T>
    inline T max(const std::vector<T> &container)
    {
        assert(container.size() > 0);
        T max = container[0];  // should be copy construct
        for (size_t i = 1; i < container.size(); ++i)
            max = std::max(max, container[i]);
        return max;
    }

    template <typename T>
    inline T min(const std::vector<T> &container)
    {
        assert(container.size() > 0);
        T min = container[0];  // should be copy construct
        for (size_t i = 1; i < container.size(); ++i)
            min = std::min(min, container[i]);
        return min;
    }

}  // namespace vec
}  // namespace sxs

#include <algorithm>
#include <vector>

template <typename T>
inline long long int
indexOf(const std::vector<T> &vector, const T &data, bool throw_exception = false)
{
    auto find_result = std::find(vector.begin(), vector.end(), data);
    if (find_result != vector.end())
    {
        return std::distance(vector.begin(), find_result);
    }
    if (throw_exception)
        throw std::runtime_error("Unable to find target from the given vector");
    return -1;
}

namespace sxs
{
// template <class T> void doNotOptimizeAway(T &&datum) {
//  asm volatile("" : "+r"(datum));
//}
template <class T>
inline auto doNotOptimizeAway(const T &datum)
{
    return reinterpret_cast<const volatile char &>(datum);
}

/*
 * Auto string stream for format string and able to cast to string
 */
class StringFormatter
{
public:
    StringFormatter()
    {
    }

    StringFormatter(const StringFormatter &other)
    {
        ss << std::string(other);
    }

    template <typename T>
    StringFormatter &operator<<(T thing)
    {
        ss << thing;
        return *this;
    }

    operator std::string() const
    {
        return ss.str();
    }

    bool print() const
    {
        sxs::println(operator std::string());
        return false;
    }

    friend std::ostream &operator<<(std::ostream &_stream, const StringFormatter &thing)
    {
        _stream << std::string(thing);  // .to_string();
        return _stream;
    }

private:
    std::stringstream ss;
};

}  // end of namespace sxs
