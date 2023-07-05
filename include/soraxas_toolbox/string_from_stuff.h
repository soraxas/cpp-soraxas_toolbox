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

#include "string.h"

#include <cxxabi.h>
#include <typeindex>

// functions for converting stuff to string
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
