#pragma once

#if __cplusplus >= 201703L && __has_include(<filesystem>)

#include <filesystem>
#include <variant>

#else // < C++17

#include "soraxas_toolbox/external/filesystem/filesystem.hpp"
#include "soraxas_toolbox/external/variant.hpp"

namespace std {

using mpark::get;
using mpark::variant;
using mpark::visit;

using namespace ghc;

// for usage in pprint
template <class...> using void_t = void;

template <class T> constexpr bool is_enum_v = is_enum<T>::value;

template <class T> constexpr bool is_signed_v = is_signed<T>::value;

} // namespace std

#endif
