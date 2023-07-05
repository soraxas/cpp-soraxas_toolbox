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

#include "token.h"

#include <iterator>

namespace sxs
{

    template <typename Token>
    using TimeStamperStatIteratorReturnType = std::tuple<Token, Token, double>;
    //
    //    // member typedefs provided through inheriting from std::iterator
    //    template <typename Token>
    //    class TimeStamperStatIterator : public std::iterator<
    //            std::input_iterator_tag,   // iterator_category
    //            TimeStamperStatIteratorReturnType<Token>,                      // value_type
    //            TimeStamperStatIteratorReturnType<Token>,                      // difference_type
    //            const TimeStamperStatIteratorReturnType<Token>,               // pointer
    //            TimeStamperStatIteratorReturnType<Token>                       // reference
    ////                long,                      // value_type
    ////                long,                      // difference_type
    ////                const long *,               // pointer
    ////                long                       // reference
    //    > {
    //    public:
    //        virtual iterator &operator++() = 0;
    //
    //        virtual iterator operator++(int) = 0;
    //
    //        virtual bool operator==(iterator other) const = 0;
    //
    //        virtual bool operator!=(iterator other) const = 0;
    //
    //        virtual reference operator*() const = 0;
    //    };

}  // namespace sxs
