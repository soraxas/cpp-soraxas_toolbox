#pragma once

#include "forward_declare.h"
#include "string.h"

#include <array>
#include <cxxabi.h>
#include <iostream>
#include <map>
#include <string>
#include <typeindex>
#include <vector>

namespace sxs
{
    namespace string
    {

        inline std::string extract_container_name(const std::string &full_container_name)
        {
            int first_open_bracket = full_container_name.find('<');
            if (first_open_bracket == std::string::npos)
                return full_container_name;
            return full_container_name.substr(0, first_open_bracket);
        }

        inline std::string extract_contained_value(const std::string &full_container_name)
        {
            int first_open_bracket = full_container_name.find('<');
            int last_close_bracket = full_container_name.rfind('>');
            if (first_open_bracket == std::string::npos || last_close_bracket == std::string::npos)
                return "";
            if (!(first_open_bracket < last_close_bracket))
            {
                // ill format of given name. abort.
                return "";
            }

            return full_container_name.substr(
                first_open_bracket + 1, last_close_bracket - (first_open_bracket + 1)
            );
        }

        inline std::string __demangle_name(const char *name)
        {
            int status;
            char *demangled = abi::__cxa_demangle(name, NULL, NULL, &status);
            std::string demangled_str{demangled};
            if (status)
                free(demangled);
            return demangled_str;
        }

        inline std::string get_type_name(const std::type_index &info)
        {
            return __demangle_name(info.name());
        }

        inline std::string get_type_name(const std::type_info &info)
        {
            return __demangle_name(info.name());
        }

        template <typename T>
        std::string get_type_name(const T &var)
        {
            return get_type_name(typeid(var));
        }

        inline bool _simplify_type_name_mapping(std::string &container_name)
        {
            // return value indicates should the templated type be skipped
            if (sxs::string::startsWith(container_name, "std::"))
            {
                container_name = container_name.substr(5);
            }
            if (container_name == "vector")
                container_name = "vec";
            else if (container_name == "pair")
                container_name = "p";
            else if (container_name == "allocator")
            {
                container_name = "";
                return true;
            }
            else if (container_name.find("basic_string") != std::string::npos)
            {
                container_name = "str";
                return true;
            }
            return false;
        }

        // forward declare
        inline void simplify_type_name_inplace(std::string &type_name);

        inline std::string _simplify_type_name__contained_value(std::string contained_name)
        {
            std::vector<std::string> parts =
                sxs::string::split_if_not_inside(contained_name, ',', '<', '>');

            std::for_each(parts.begin(), parts.end(), sxs::string::trim);

            std::vector<std::string> out;
            out.reserve(parts.size());
            // skip empyt parts
            for (auto &&p : parts)
            {
                simplify_type_name_inplace(p);
                if (p != "")
                    out.emplace_back(p);
            }

            return sxs::string::join(out, ",");
        }

        inline void simplify_type_name_inplace(std::string &type_name)
        {
            std::string first_half = extract_container_name(type_name);
            if (_simplify_type_name_mapping(first_half))
            {
                // skip the templated type
                type_name = first_half;
                return;
            }

            std::string second_half = extract_contained_value(type_name);
            if (second_half.size() == 0)
            {
                type_name = first_half;
            }
            else
            {
                type_name =
                    first_half + "<" + _simplify_type_name__contained_value(second_half) + ">";
            }
        }

        inline std::string simplify_type_name(std::string type_name)
        {
            simplify_type_name_inplace(type_name);
            return type_name;
        }

        inline std::string simplify_type_name(const std::type_info &info)
        {
            return simplify_type_name(get_type_name(info));
        }

    }  // namespace string
};     // namespace sxs

// template for printing type info
inline std::ostream &operator<<(std::ostream &out, const std::type_info &info)
{
    return out << sxs::string::simplify_type_name(sxs::string::get_type_name(info));
}

inline std::ostream &operator<<(std::ostream &out, const std::type_index &type_index)
{
    return out << sxs::string::simplify_type_name(sxs::string::get_type_name(type_index));
}

///////////////////////////////////
// template for printing pair
template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &out, const typename std::pair<T1, T2> &pair)
{
    return out << "(" << pair.first << "," << pair.second << ")";
}

///////////////////////////////////
// template for printing tuple
namespace sxs
{
    template <class Tuple, std::size_t N>
    void __print_tuple(std::ostream &out, const Tuple &t)
    {
        if constexpr (N > 1)
        {
            __print_tuple<Tuple, N - 1>(out, t);
            out << ", ";
        }
        out << std::get<N - 1>(t);
    }
}  // namespace sxs

template <class... Args>
std::ostream &operator<<(std::ostream &out, const std::tuple<Args...> &t)
{
    using namespace sxs::string;
    out << simplify_type_name(get_type_name(t)) << "(";
    sxs::__print_tuple<decltype(t), sizeof...(Args)>(out, t);
    return out << ")";
}

///////////////////////////////////

//// template for printing any iterable container
// disable this template for std::string
template <
    class IterableContainer, class Begin = decltype(std::begin(std::declval<IterableContainer>())),
    typename
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
    ,
    typename
#endif
    >

inline std::ostream &operator<<(std::ostream &stream, const IterableContainer &vect)
{
    using namespace sxs::string;

    // retrieve datatype of the iterable
    using IterableContainerIteratorType = decltype(std::begin(std::declval<IterableContainer>()));
    using ValueType = typename std::iterator_traits<IterableContainerIteratorType>::value_type;

    const char *dlm = "";
    //    std::string container_type_name =
    //            simplify_type_name(extract_container_name(get_type_name(typeid(IterableContainer))));
    std::string value_type_name = simplify_type_name(get_type_name(typeid(ValueType)));
    if (value_type_name != "double" && value_type_name != "int" && value_type_name != "float")
    {
        stream << simplify_type_name(typeid(IterableContainer));
    }
    stream << "[";
    for (const auto &i : vect)
    {
        stream << dlm << i;
        dlm = ", ";
    }
    stream << "]";

    return stream;
}

namespace sxs
{
    // return a pointer to a referenced ostream
    inline static std::ostream *&get_print_output_stream()
    {
        static std::ostream *ostream{&std::cout};
        return ostream;
    }

    inline void print_flush()
    {
        std::flush(*get_print_output_stream());
    }

    // variadic print function
    template <typename T1>
    inline void print(const T1 &first)
    {
        *get_print_output_stream() << first;
    }

    // specialisation
    template <>
    inline void print(const bool &boolean)
    {
        if (boolean)
            print("true");
        else
            print("false");
    }

    template <typename T1, typename... T2>
    inline void print(const T1 &first, const T2 &...rest)
    {
        print(first);
        print(rest...);
    }

    template <typename T1>
    inline void print_spaced(const T1 &first)
    {
        print(first);
    }

    /*
     *  Print a comma separated list of any items, with space in-between
     * */
    template <typename T1, typename... T2>
    inline void print_spaced(const T1 &first, const T2 &...rest)
    {
        print(first, " ");
        print_spaced(rest...);
    }

    /*
     *  Print an empty line
     * */
    template <typename... T>
    inline void println()
    {
        *get_print_output_stream() << std::endl;
    }

    /*
     *  Print a comma separated list of any items
     * */
    template <typename... T>
    inline void println(const T &...rest)
    {
        print(rest...);
        println();
    }

    /*
     *  Print a comma separated list of any items, with space in-between
     * */
    template <typename... T>
    inline void println_spaced(const T &...rest)
    {
        print_spaced(rest...);
        println();
    }

}  // namespace sxs

#ifdef SXS_RUN_TESTS
/*
 * -------------------------------------------
 * Test cases and general usage for this file:
 * -------------------------------------------
 */

TEST_CASE("[sxs] Test printing to ostream")
{
    using namespace sxs;

    // assign print output stream to our ostream
    std::ostringstream oss;

    const auto _original_ostream = sxs::get_print_output_stream();
    sxs::get_print_output_stream() = &oss;

    SUBCASE("[sxs] no newline")
    {
        print("Hello world");
        CHECK(oss.str() == "Hello world");
    }

    SUBCASE("[sxs] newline")
    {
        println("Hello world");
        CHECK(oss.str() == "Hello world\n");
    }

    SUBCASE("[sxs] spaced print")
    {
        println_spaced("Hello world", 3, "/", 10);
        CHECK(oss.str() == "Hello world 3 / 10\n");
    }

    SUBCASE("[sxs] semantic printing")
    {
        SUBCASE("")
        {
            print(true);
            CHECK(oss.str() == "true");
        }
        SUBCASE("")
        {
            print(false);
            CHECK(oss.str() == "false");
        }
        SUBCASE("")
        {
            print(1.5);
            CHECK(oss.str() == "1.5");
        }
        SUBCASE("")
        {
            print(1.0);
            CHECK(oss.str() == "1");
        }
        SUBCASE("")
        {
            print(5.91856f);
            CHECK(oss.str() == "5.91856");
        }
    }

    SUBCASE("[sxs] container printing")
    {
        SUBCASE("trivial container")
        {
            std::vector<int> vec = {1, 6, 8};
            print(vec);
            CHECK(oss.str() == "[1, 6, 8]");
        }
        SUBCASE("trivial container")
        {
            std::vector<double> vec = {1.5, 6.9, 8.85};
            print(vec);
            CHECK(oss.str() == "[1.5, 6.9, 8.85]");
        }
        SUBCASE("non-trivial container")
        {
            std::vector<std::pair<double, int>> vec = {{1.5, 8}, {6.9, 98}, {8.85, -84}};
            print(vec);
            CHECK(oss.str() == "vec<p<double,int>>[(1.5,8), (6.9,98), (8.85,-84)]");
        }
    }

    SUBCASE("[sxs] print type")
    {
        SUBCASE("")
        {
            print(typeid(int));
            CHECK(oss.str() == "int");
        }
        SUBCASE("")
        {
            print(typeid(double));
            CHECK(oss.str() == "double");
        }
        SUBCASE("")
        {
            print(typeid(float));
            CHECK(oss.str() == "float");
        }
        SUBCASE("")
        {
            print(typeid(bool));
            CHECK(oss.str() == "bool");
        }
        SUBCASE("")
        {
            print(typeid(std::pair<int, double>));
            CHECK(oss.str() == "p<int,double>");
        }
        SUBCASE("")
        {
            print(typeid(std::string));
            CHECK(oss.str() == "str");
        }
    }

    sxs::get_print_output_stream() = _original_ostream;
}

#endif  // SXS_RUN_TESTS
