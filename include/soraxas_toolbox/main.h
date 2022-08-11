#pragma once

#include "assert.h"
#include "forward_declare.h"
#include "print_utils.h"

#include <cmath>     // for std::sqrt
#include <iterator>  // needed for std::ostram_iterator
#include <string>

#define SXS_USE_PPRINT

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

#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>

namespace sxs
{
    inline double compute_sum(std::vector<double> nums)
    {
        return std::accumulate(nums.begin(), nums.end(), 0.);
    }

    inline std::pair<double, double> compute_mean_and_stdev(std::vector<double> nums)
    {
        double sum = compute_sum(nums);
        double mean = sum / nums.size();

        std::vector<double> diff(nums.size());
        std::transform(
            nums.begin(), nums.end(), diff.begin(), [mean](double x) { return x - mean; }
        );
        double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
        double stdev = std::sqrt(sq_sum / nums.size());
        return {mean, stdev};
    }

    ////////////////////////////////////////////////////////////

    inline std::pair<double, std::string>
    _get_time_factor_and_unit(double elapsed, bool fix_width = false)
    {
        double factor;
        std::string unit;
        if (elapsed < 1e-6)
        {
            factor = 1e9;
            unit = "ns";
        }
        else if (elapsed < 1e-3)
        {
            factor = 1e6;
            unit = "µs";
        }
        else if (elapsed < 1)
        {
            factor = 1e3;
            unit = "ms";
        }
        else
        {
            factor = 1;
            unit = "s";
            if (fix_width)
                unit += " ";
        }
        return {factor, unit};
    }

#define _FIX_WIDTH_DECIMAL(precision) /* +1 is for the decimal point */                            \
    std::setprecision(precision) << std::left << std::setw(precision + 1)

    inline std::string format_time2readable(double elapsed, int precision = 3)
    {
        auto factor_and_unit = _get_time_factor_and_unit(elapsed, true);
        std::stringstream ss;
        ss << _FIX_WIDTH_DECIMAL(precision) << (elapsed * factor_and_unit.first)
           << factor_and_unit.second;
        return ss.str();
    }

    inline std::string format_time2readable(std::vector<double> all_elapsed, int precision = 3)
    {
        auto mean_stdev = compute_mean_and_stdev(all_elapsed);
        // use mean to get factor and units
        auto factor_and_unit = _get_time_factor_and_unit(mean_stdev.first, true);
        std::stringstream ss;
        ss << _FIX_WIDTH_DECIMAL(precision) << (mean_stdev.first * factor_and_unit.first) << "±"
           << _FIX_WIDTH_DECIMAL(precision) << (mean_stdev.second * factor_and_unit.first)
           << factor_and_unit.second;
        return ss.str();
    }
}  // namespace sxs

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

    template <typename Lambda>
    void timeit(const Lambda &lambda, const std::string title = "untitled")
    {
        typedef std::chrono::high_resolution_clock clock_;
        typedef std::chrono::duration<double, std::ratio<1>> second_;
        auto f = [&lambda]()
        {
            std::chrono::time_point<clock_> start = clock_::now();
            doNotOptimizeAway(lambda());
            return std::chrono::duration_cast<second_>(clock_::now() - start).count();
        };

        double _elapsed = 0;
        unsigned long loop_count = 0;
        double fastest, slowest;
        auto print_message = [&loop_count, &title, &_elapsed, &fastest, &slowest]()
        {
            std::cout << "========================================" << std::endl;
            std::cout << "[" << title << "]: Loop: " << loop_count
                      << " | Total time: " << sxs::format_time2readable(_elapsed) << " | Avg: "
                      << sxs::format_time2readable(_elapsed / static_cast<double>(loop_count))
                      << " (" << sxs::format_time2readable(fastest) << " ~ "
                      << sxs::format_time2readable(slowest) << ")" << std::endl;
            std::cout << "========================================" << std::endl;
        };
        // first time
        // run it one time to test how long it takes
        _elapsed += f(), ++loop_count;
        fastest = slowest = _elapsed;

        auto run_until_loop_count =
            [&loop_count, &f, &_elapsed, &fastest, &slowest](unsigned long N)
        {
            while (loop_count < N)
            {
                double time_taken = f();
                ++loop_count;
                _elapsed += time_taken;
                fastest = std::min(fastest, time_taken);
                slowest = std::max(slowest, time_taken);
            }
        };

        if (_elapsed > 60)
        {
            // too long, stop right away.
            print_message();
            return;
        }
        else if (_elapsed > 10)
        {
            // run 5 times
            run_until_loop_count(5);
            print_message();
            return;
        }
        else if (_elapsed > 1)
        {
            // run 10 times
            run_until_loop_count(10);
            print_message();
            return;
        }
        else if (_elapsed > 0.1)
        {
            // run 20 times
            run_until_loop_count(20);
            print_message();
            return;
        }
        else if (_elapsed > 0.01)
        {
            // run 100 times
            run_until_loop_count(100);
            print_message();
            return;
        }

        // micro benchmark:
        run_until_loop_count(100);
        if (_elapsed < 3)
        {
            run_until_loop_count(1000);
            if (_elapsed < 3)
            {
                run_until_loop_count(10000);
                if (_elapsed < 3)
                {
                    run_until_loop_count(100000);
                    if (_elapsed < 3)
                    {
                        run_until_loop_count(1000000);
                    }
                }
            }
        }
        print_message();
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
