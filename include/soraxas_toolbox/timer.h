#pragma once

#include "main.h"

#include "external/ordered-map/ordered_map.h"

#include <iostream>

namespace sxs
{
    class TimeStamper : public Timer
    {
        using Timer::clock_;

    public:
        TimeStamper(const std::string &name = "", bool print_starter = false,
                    bool auto_print = true)
          : name(name)
          , m_autoprint(auto_print)
          , m_counts(0)
          , m_finished(false)
          , m_print_starter(print_starter)
        {
            if (m_print_starter)
                std::cout << "[" << name << "] (computing...)" << std::flush;
        }

        ~TimeStamper()
        {
            finish();
        }

        void finish()
        {
            if (m_finished)
                return;
            m_finished = true;
            if (m_autoprint)
            {
                if (m_print_starter)
                    std::cout << '\r';  // erase line
                if (!stamped.empty())
                    print_stamped_stats();
                else
                    // run the current class' string operator
                    std::cout << operator std::string() << std::endl;
            }
        }

        void stamp(std::string &&stamp_string)
        {
            if (!_last_stamped_string.empty())
            {
                //      std::stringstream ss;
                //      ss << _last_stamped_string << " -> " << stamp_string;
                stamped[{_last_stamped_string, stamp_string}].push_back(
                    timepoint_diff_to_secs(clock_::now() - _last_stamped_clock));
            }
            _last_stamped_clock = clock_::now();
            _last_stamped_string = std::move(stamp_string);
        }

        void print_stamped_stats()
        {
            using sxs::format_time2readable;
            size_t string_1_max_len = 0;
            size_t string_2_max_len = 0;
            size_t stats_size_max_len = 0;
            double total_time_spent = 0;
            for (auto &&item : stamped)
            {
                string_1_max_len = std::max(string_1_max_len, item.first.first.length());
                string_2_max_len = std::max(string_2_max_len, item.first.second.length());
                stats_size_max_len =
                    std::max(stats_size_max_len, std::to_string(item.second.size()).length());
                total_time_spent += sxs::compute_sum(item.second);
            }
            std::cout << "========== stamped result ==========" << std::endl;
            for (auto &&item : stamped)
            {
                double max = item.second[0];
                double min = item.second[0];
                std::for_each(item.second.begin(), item.second.end(),
                              [&max, &min](double val)
                              {
                                  max = std::max(val, max);
                                  min = std::min(val, min);
                              });
                double sum = sxs::compute_sum(item.second);
                std::cout << std::left                                         //
                          << std::setw(string_1_max_len) << item.first.first   // from
                          << " -> "                                            //
                          << std::setw(string_2_max_len) << item.first.second  // to
                          << ": " << format_time2readable(item.second)         // mean, stdev
                          << " (" << format_time2readable(min) << "~"          // min
                          << format_time2readable(max) << ")"                  // max
                          << " [Σ^" << std::setw(stats_size_max_len)
                          << item.second.size()  // number of collected stats size
                          << "=" << format_time2readable(sum) << "|"  // sum
                          << _FIX_WIDTH_DECIMAL(3) << (sum / total_time_spent * 100) << "%]"
                          << std::endl;  // percentage of time spent
            }
            std::cout << "====================================" << std::endl;
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

        friend std::ostream &operator<<(std::ostream &_stream, const TimeStamper &t)
        {
            _stream << std::string(t);  // .to_string();
            return _stream;
        }

        template <class... T>
        static TimeStamper auto_print(T... args)
        {
            TimeStamper timer = TimeStamper(args...);
            timer.set_autoprint();
            return timer;
        }

        void add_count(int counts)
        {
            m_counts += counts;
        }

        void set_autoprint()
        {
            m_autoprint = true;
        }

    private:
        bool m_finished;

        struct pair_hash
        {
            template <class T1, class T2>
            std::size_t operator()(const std::pair<T1, T2> &pair) const
            {
                return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
            }
        };

        tsl::ordered_map<std::pair<std::string, std::string>, std::vector<double>, pair_hash>
            stamped;
        std::string _last_stamped_string;
        clock_::time_point _last_stamped_clock;
        std::string name;
        bool m_autoprint;
        int m_counts;
        bool m_print_starter;
    };
}  // namespace sxs
