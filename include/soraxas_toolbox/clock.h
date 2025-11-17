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

#include "format.h"

#include <chrono>

namespace sxs
{

class Timer
{
    using FloatType = float;

protected:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<FloatType, std::ratio<1>> second_;
    clock_::time_point beg_;

public:
    Timer() : beg_(clock_::now())
    {
    }

    virtual void reset()
    {
        beg_ = get_timepoint();
    }

    virtual FloatType elapsed() const
    {
        return timepoint_diff_to_secs(clock_::now() - beg_);
    }

    inline static clock_::time_point get_timepoint()
    {
        return clock_::now();
    }

    static FloatType timepoint_diff_to_secs(const clock_::duration &duration)
    {
        return std::chrono::duration_cast<second_>(duration).count();
    }

    static FloatType timepoint_diff_to_secs(const clock_::time_point &tp)
    {
        return timepoint_diff_to_secs(get_timepoint() - tp);
    }
};

/*
 * Given something callable, this function will call the callable while
 * throttling of only calling it every x second.
 * E.g., for printing.
 */
template <long uuid = 0, typename F>
inline void throttle(const float every_x_second, const F &functor)
{
    static sxs::Timer timer = [&functor]()
    {
        // call functor when we first initialise
        functor();
        return std::move(sxs::Timer{});
    }();
    if (timer.elapsed() >= every_x_second)
    {
        functor();
        timer.reset();
    }
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
                  << sxs::format_time2readable(_elapsed / static_cast<double>(loop_count)) << " ("
                  << sxs::format_time2readable(fastest) << " ~ "
                  << sxs::format_time2readable(slowest) << ")" << std::endl;
        std::cout << "========================================" << std::endl;
    };
    // first time
    // run it one time to test how long it takes
    _elapsed += f(), ++loop_count;
    fastest = slowest = _elapsed;

    auto run_until_loop_count = [&loop_count, &f, &_elapsed, &fastest, &slowest](unsigned long N)
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
}  // namespace sxs

#ifdef SXS_RUN_TESTS___
/*
 * -------------------------------------------
 * Test cases and general usage for this file:
 * -------------------------------------------
 */

TEST_CASE("[sxs] Test timing")
{
    using namespace sxs;

    SUBCASE("test timer")
    {
        Timer timer{};

        CHECK(timer.elapsed() > 0);
    }

    int count = 0;
    SUBCASE("test throttle long delay")
    {
        count = 0;
        for (int i = 0; i < 100; ++i)
        {
            throttle(1, [&count]() { ++count; });
        }
        CHECK(count == 1);
    }

    SUBCASE("test throttle short delay")
    {
        count = 0;
        for (int i = 0; i < 100; ++i)
        {
            throttle(1e-99, [&count]() { ++count; });
        }
        CHECK(count >= 100);  // count should be 101 or 100
        CHECK(count <= 101);  // count should be 101 or 100
    }

    // sxs::get_print_output_stream() = _original_ostream;
}

#endif  // SXS_RUN_TESTS