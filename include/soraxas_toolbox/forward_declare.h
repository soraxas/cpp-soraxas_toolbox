#pragma once

#include <iostream>

//// template for printing any iterable container
// disable this template for std::string
template <
    class IterableContainer, class Begin = decltype(std::begin(std::declval<IterableContainer>())),
    class ValueType = typename std::iterator_traits<Begin>::value_type,
    typename =
        typename std::enable_if<!std::is_base_of<IterableContainer, std::string>::value>::type
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
    ,
    typename =
        typename std::enable_if<!std::is_base_of<IterableContainer, std::string_view>::value>::type
#endif
    >
inline std::ostream &operator<<(std::ostream &stream, const IterableContainer &vect);
