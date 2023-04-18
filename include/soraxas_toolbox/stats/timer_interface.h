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
