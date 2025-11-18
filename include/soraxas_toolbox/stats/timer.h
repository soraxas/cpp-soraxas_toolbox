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

#include "timer_interface.h"
#include "timing.h"

#include "../clock.h"
#include "../external/ordered-map/ordered_map.h"
#include "../print_utils_core.h"

namespace sxs
{

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &pair) const
    {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

}  // namespace sxs

namespace sxs
{

template <typename Token>
using TimeStampCollectionKey = std::pair<Token, Token>;

template <typename Token>
using TimeStampCollection =
    tsl::ordered_map<TimeStampCollectionKey<Token>, Stats<double>, pair_hash>;

/*
 * Compile results from interator that returns <token_e1, token_e2, double>
 * */
template <
    typename ResultDataType = double, typename IteratorContainer,
    typename Token = typename std::tuple_element<0, typename IteratorContainer::stat_t>::type>
TimeStampCollection<Token> compile_result(IteratorContainer &container)
{
    static_assert(
        std::is_same<
            typename std::tuple_element<0, typename IteratorContainer::stat_t>::type,
            typename std::tuple_element<1, typename IteratorContainer::stat_t>::type>::value,
        "Inconsistent token type for tuple element 1 & 2"
    );

    tsl::ordered_map<std::pair<Token, Token>, Stats<ResultDataType>, pair_hash> result{};

    tsl::ordered_map<std::pair<Token, Token>, std::vector<double>, pair_hash> tmp_result{};

    for (auto &&item : container)
    {
        auto key =
            std::make_pair<Token, Token>(((Token)std::get<0>(item)), ((Token)std::get<1>(item)));
        auto &stat = result[key];

        const double val = std::get<2>(item);
        //                    stat.count++;
        stat.min = std::min(val, stat.min);
        stat.max = std::max(val, stat.max);
        stat.sum += val;
        tmp_result[key].push_back(val);
    }

    for (auto &&tmp_item : tmp_result)
    {
        auto &stat = result[tmp_item.first];
        auto &tmp_collected_vals = tmp_item.second;
        stat.count = tmp_collected_vals.size();
        stat.mean() = stat.sum / stat.count;
        stat.stdev() = sxs::compute_stdev(stat.mean(), tmp_collected_vals);
    }

    //                result[{std::get<0>(stat),
    //                std::get<1>(stat)}].push_back(std::get<2>(stat));

    return result;
}

template <typename Token>
void print_compiled_stats(
    const TimeStampCollection<Token> &stamped,
    const std::function<std::string(Token)> &to_string_functor = 0
)
{
    using sxs::format_time2readable;
    size_t string_1_max_len = 0;
    size_t string_2_max_len = 0;
    size_t stats_size_max_len = 0;
    double total_time_spent = 0;

    tsl::ordered_map<Token, std::string> token_to_str;
    for (auto &&item : stamped)
    {
        const std::pair<Token, Token> &key = item.first;
        if (to_string_functor)
        {
            if (token_to_str.count(key.first) == 0)
                token_to_str[key.first] = to_string_functor(key.first);
            if (token_to_str.count(key.second) == 0)
                token_to_str[key.second] = to_string_functor(key.second);
        }
        else
        {
            if (token_to_str.count(key.first) == 0)
                token_to_str[key.first] = sxs::stats::get_token_name(key.first);
            if (token_to_str.count(key.second) == 0)
                token_to_str[key.second] = sxs::stats::get_token_name(key.second);
        }

        string_1_max_len = std::max(string_1_max_len, token_to_str[key.first].length());
        string_2_max_len = std::max(string_2_max_len, token_to_str[key.second].length());
        stats_size_max_len =
            std::max(stats_size_max_len, std::to_string(item.second.count).length());

        total_time_spent += item.second.sum;
    }
    sxs::println("========== stamped result ==========");
    for (auto &&item : stamped)
    {
        const Token token_from = item.first.first;
        const Token token_to = item.first.second;

        *sxs::get_print_output_stream()
            << std::left                                                //
            << std::setw(string_1_max_len) << token_to_str[token_from]  // from
            << " -> "                                                   //
            << std::setw(string_2_max_len) << token_to_str[token_to]    // to
            << ": " << format_time2readable(item.second.mean_stdev)     // mean, stdev
            << " (" << format_time2readable(item.second.min) << "~"     // min
            << format_time2readable(item.second.max) << ")"             // max
            << " [Σ^" << std::setw(stats_size_max_len)
            << item.second.count  // number of collected stats size
            << "=" << format_time2readable(item.second.sum) << "|"  // sum
            << _FIX_WIDTH_DECIMAL(3) << (item.second.sum / total_time_spent * 100) << "%]"
            << std::endl;  // percentage of time spent
    }
    sxs::println("====================================");
}

template <typename Token, typename F>
inline void print_compiled_stats(const TimeStampCollection<Token> &stamped, F to_string_functor)
{
    // to_string_functor can be anything callable
    static_assert(
        std::is_same<decltype(to_string_functor(std::declval<Token>())), std::string>::value
    );

    std::function<std::string(Token)> functor = to_string_functor;

    print_compiled_stats(stamped, functor);
}

template <typename Token, typename F>
void print_aggregated_stamped_stats(
    const std::vector<TimeStampCollection<Token>> &all_stamped, F to_string_functor
)
{
    TimeStampCollection<Token> aggregated_stamped{};

    tsl::ordered_map<TimeStampCollectionKey<Token>, std::vector<double>, pair_hash>
        aggregated_stamped_tmp;

    for (auto &&stamped : all_stamped)
    {
        for (auto &&item : stamped)
        {
            aggregated_stamped[item.first].accumulate_without_mean_stdev(item.second);
            aggregated_stamped_tmp[item.first].push_back(item.second.mean());
        }
    }
    // second pass to compute mean and stdev

    for (auto &&stamped : aggregated_stamped)
    {
        auto &&item = aggregated_stamped[stamped.first];
        item.mean() = stamped.second.sum / stamped.second.count;
        item.stdev() =
            sxs::compute_stdev(stamped.second.mean(), aggregated_stamped_tmp[stamped.first]);
    }
    print_compiled_stats(aggregated_stamped, to_string_functor);
}

/*
 * A helper function that auto add to-string function
 */
template <typename Token>
void print_aggregated_stamped_stats(const std::vector<TimeStampCollection<Token>> &all_stamped)
{
    const std::function<std::string(Token)> to_string_functor = 0;
    print_aggregated_stamped_stats(all_stamped, to_string_functor);
}

/*
 * A helper function that auto compile results
 */
template <
    typename CompilableTimeStamper,
    typename Token = typename std::tuple_element<0, typename CompilableTimeStamper::stat_t>::type,
    typename... Args>
void print_aggregated_stamped_stats(
    const std::vector<CompilableTimeStamper> &time_stameprs, Args... args
)
{
    std::vector<TimeStampCollection<Token>> all_stamped;
    all_stamped.reserve(time_stameprs.size());
    bool valid = true;
    for (auto &&time_stamper : time_stameprs)
    {
        if (valid)
            // once something reported invalid, stop checking to avoid being over-flooded with
            // messages
            valid &= time_stamper.check_validity();
        all_stamped.push_back(sxs::compile_result(time_stamper));
    }
    print_aggregated_stamped_stats(all_stamped, std::forward<Args>(args)...);
}
}  // namespace sxs

namespace sxs
{

template <typename Token>
class TimeStamperBase : public Timer
{
protected:
    using Timer::clock_;

public:
    using stat_t = TimeStamperStatIteratorReturnType<Token>;

    TimeStamperBase(
        const std::string &name = "", bool print_starter = false, bool auto_print = true
    )
      : name(name)
      , m_autoprint(auto_print)
      , m_counts(0)
      , m_finished(false)
      , m_print_starter(print_starter)
    {
        if (m_print_starter)
            *sxs::get_print_output_stream() << "[" << name << "] (computing...)" << std::flush;
    }

    ~TimeStamperBase()
    {
        finish();
    }

    size_t count()
    {
        return stamped.size();
    }

    void reset()
    {
        stamped.clear();
        _last_stamped_token = Token{};
    }

    void finish()
    {
        if (m_finished)
            return;
        m_finished = true;
        if (m_autoprint)
        {
            if (m_print_starter)
                *sxs::get_print_output_stream() << '\r';  // erase line
            if (!stamped.empty())
                print_stamped_stats();
            else
                // run the current class' string operator
                *sxs::get_print_output_stream() << operator std::string() << std::endl;
        }
    }

    void print_stamped_stats()
    {
        print_compiled_stats(compile_result(*this));
    }

    operator std::string() const
    {
        std::stringstream ss;
        double _elapsed = elapsed();
        ss << "[" << name << "] elapsed: " << sxs::format_time2readable(_elapsed);
        if (m_counts > 0)
        {
            ss << " avg: ";
            ss << sxs::format_time2readable(_elapsed / m_counts);
            ss << "/it";
            ss << " [out of " << m_counts << "]";
        }
        return ss.str();
    }

    friend std::ostream &operator<<(std::ostream &_stream, const TimeStamperBase &t)
    {
        return _stream << std::string(t);
    }

    template <class... T>
    static TimeStamperBase auto_print(T... args)
    {
        TimeStamperBase timer = TimeStamperBase(args...);
        timer.set_autoprint();
        return timer;
    }

    void add_count(int counts)
    {
        m_counts += counts;
    }

    void set_autoprint(bool autoprint = true)
    {
        m_autoprint = autoprint;
    }

    bool m_finished;

    class iterator
    {
        const std::vector<TimeStamperStatIteratorReturnType<Token>> &stamped_ref_;
        long num;

    public:
        using iterator_category = std::output_iterator_tag;
        using value_type = stat_t;
        using difference_type = stat_t;
        using pointer = const stat_t *;
        using reference = stat_t;

        explicit iterator(
            const std::vector<TimeStamperStatIteratorReturnType<Token>> &ref, long _num
        )
          : stamped_ref_(ref), num(_num)
        {
        }

        iterator &operator++()
        {
            num++;
            return *this;
        }

        iterator operator++(int)
        {
            iterator retval = *this;
            ++(*this);
            return retval;
        }

        bool operator==(iterator other) const
        {
            return num == other.num;
        }

        bool operator!=(iterator other) const
        {
            return !(*this == other);
        }

        stat_t operator*() const
        {
            return stamped_ref_[num];
        }
    };

    iterator begin() const
    {
        return iterator(stamped, 0);
    }

    iterator end() const
    {
        return iterator(stamped, stamped.size());
    }

    bool check_validity() const
    {
        return true;
    }

protected:
    std::vector<TimeStamperStatIteratorReturnType<Token>> stamped;

    Token _last_stamped_token;
    clock_::time_point _last_stamped_clock;
    std::string name;
    bool m_autoprint;
    int m_counts;
    bool m_print_starter;
};

class TimeStamperDynamic : public TimeStamperBase<std::string>
{
public:
    void stamp(const std::string &stamp_string)
    {
        if (!_last_stamped_token.empty())
        {
            stamped.emplace_back(
                _last_stamped_token, stamp_string,
                timepoint_diff_to_secs(clock_::now() - _last_stamped_clock)
            );
        }
        _last_stamped_clock = clock_::now();
        _last_stamped_token = std::move(stamp_string);
    }
};

template <typename Token>
class TimeStamper : public TimeStamperBase<Token>
{
private:
    using self = TimeStamperBase<Token>;

protected:
    bool is_first = true;

public:
    template <Token t>
    void stamp()
    {
        if (!is_first)
        {
            self::stamped.emplace_back(
                self::_last_stamped_token, t,
                self::timepoint_diff_to_secs(self::clock_::now() - self::_last_stamped_clock)
            );
        }
        is_first = false;
        self::_last_stamped_clock = self::clock_::now();
        self::_last_stamped_token = std::move(t);
    }
};

}  // namespace sxs

#ifdef SXS_RUN_TESTS
/*
 * -------------------------------------------
 * Test cases and general usage for this file:
 * -------------------------------------------
 */

#include "soraxas_toolbox/print_utils.h"
#include "soraxas_toolbox/string.h"

namespace __sxs_timer
{

TEST_CASE("[sxs] An int vector with some items")
{
    // assign print output stream to void
    sxs::SXSPrintOutputStreamGuard guard;

    sxs::TimeStamperDynamic timer;
    timer.stamp("hi");
    timer.stamp("hbye");
    timer.stamp("hi");
    timer.stamp("hi");

    sxs::println(timer);

    print_compiled_stats(compile_result(timer));
    CHECK(sxs::string::contains(guard.oss().str(), "stamped result"));
    CHECK(sxs::string::contains(guard.oss().str(), "hbye"));
}

#ifdef SXS_HAS_ENUM_HPP
SXS_DEFINE_ENUM_AND_TRAITS(
    my_smart_enum, char,  //
    (myval1)              //
    (myval2)              //
    (myval3)              //
)

TEST_CASE("[sxs] An int vector with some items")
{
    sxs::SXSPrintOutputStreamGuard guard;

    sxs::TimeStamper<my_smart_enum> timer;
    timer.stamp<my_smart_enum::myval1>();
    timer.stamp<my_smart_enum::myval1>();
    timer.stamp<my_smart_enum::myval2>();
    timer.stamp<my_smart_enum::myval3>();
    timer.stamp<my_smart_enum::myval2>();
    timer.stamp<my_smart_enum::myval3>();

    sxs::println(timer);

    print_compiled_stats(compile_result(timer));
    CHECK(sxs::string::contains(guard.oss().str(), "myval1"));
    CHECK(sxs::string::contains(guard.oss().str(), "myval3"));
    CHECK(!sxs::string::contains(guard.oss().str(), "myval3notexists"));
}
#endif

}  // namespace __sxs_timer
#endif  // SXS_RUN_TESTS
