#ifndef SXS_STRING_H_
#define SXS_STRING_H_

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

// functions to manipulate string
namespace sxs
{
    namespace string
    {
        /**
         * Split a given string into vector of strings by delimiter
         * @param s
         * @param delimiter
         * @return
         */
        inline std::vector<std::string> split(const std::string &s, const std::string &delimiter)
        {
            size_t pos_start = 0, pos_end, delim_len = delimiter.length();
            std::string token;
            std::vector<std::string> res;

            while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
            {
                token = s.substr(pos_start, pos_end - pos_start);
                pos_start = pos_end + delim_len;
                res.push_back(token);
            }

            res.push_back(s.substr(pos_start));
            return res;
        }

        /**
         * Split a given string into vector of strings by delimiter
         * @param s
         * @param delimiter
         * @return
         */
        inline std::vector<std::string> split_if_not_inside(
            const std::string &s, const char delimiter = ',', const char open = '<',
            const char close = '>'
        )
        {
            // much SLOWER than the other!
            size_t pos = 0;
            size_t last_split = 0;
            std::vector<std::string> res;

            int nested_counter = 0;
            while (pos < s.size())
            {
                const char cur_char = s[pos];
                if (cur_char == delimiter)
                {
                    if (nested_counter == 0)
                    {
                        res.emplace_back(s.substr(last_split, pos - last_split));
                        last_split = pos + 1;  // +1 to skip the delimiter next time.
                    }
                }
                else if (cur_char == open)
                    ++nested_counter;
                else if (cur_char == close)
                    --nested_counter;
                ++pos;
            }

            res.push_back(s.substr(last_split));
            return res;
        }

        //        template <template <typename T> class Container>
        //        inline std::string join(Container &parts, const std::string &delimiter) {
        //            auto&& begin = std::begin(parts);
        //            auto&& end = std::end(parts);
        //            if (begin == end)
        //                return "";
        //            std::stringstream ss;
        //            ss << *(begin++);
        //            while(begin != end)
        //                ss << *(begin++);
        //            return ss.str();
        //        }
        template <class Vector, class Begin = decltype(std::begin(std::declval<Vector>()))>
        inline std::string join(const Vector &vect, const std::string &delimiter)
        {
            auto &&begin = std::begin(vect);
            auto &&end = std::end(vect);
            if (begin == end)
                return "";
            std::stringstream ss;
            ss << *(begin++);
            while (begin != end)
            {
                ss << delimiter;
                ss << *(begin++);
            }
            return ss.str();
        }

        inline bool contains(const std::string &main_string, const std::string &substring)
        {
            return main_string.find(substring) != std::string::npos;
        }

        inline size_t count(const std::string &main_string, const std::string &substring)
        {
            size_t occurrences = 0;
            std::string::size_type pos = 0;
            while ((pos = main_string.find(substring, pos)) != std::string::npos)
            {
                ++occurrences;
                pos += substring.length();
            }
            return occurrences;
        }

        inline bool startsWith(const std::string &s, const std::string &token)
        {
            return s.rfind(token, 0) == 0;
        }

        // followings are from https://stackoverflow.com/questions/216823/how-to-trim-an-stdstring
        // trim from start (in place)
        static inline void ltrim(std::string &s)
        {
            s.erase(
                s.begin(),
                std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); })
            );
        }

        // trim from end (in place)
        static inline void rtrim(std::string &s)
        {
            s.erase(
                std::find_if(
                    s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }
                ).base(),
                s.end()
            );
        }

        // trim from both ends (in place)
        static inline void trim(std::string &s)
        {
            rtrim(s);
            ltrim(s);
        }

        // trim from start (copying)
        static inline std::string ltrim_copy(std::string s)
        {
            ltrim(s);
            return s;
        }

        // trim from end (copying)
        static inline std::string rtrim_copy(std::string s)
        {
            rtrim(s);
            return s;
        }

        // trim from both ends (copying)
        static inline std::string trim_copy(std::string s)
        {
            trim(s);
            return s;
        }
    }  // namespace string
}  // namespace sxs

#endif  // SXS_STRING_H_
