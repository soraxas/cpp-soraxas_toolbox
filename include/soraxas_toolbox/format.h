#ifndef SXS_FORMAT_H
#define SXS_FORMAT_H

#include "vector_math.h"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace sxs
{

    const std::pair<double, const char *> &get_si_unit_and_factor(double value)
    {
        static constexpr const std::array<std::pair<double, const char *>, 8> si_units = {
            // let i be the index, then,
            // si_unit[i] is valid iff its value is betweel value_range[i] and value_range[i+1]
            std::make_pair<double, const char *>(1e9, "n"),
            std::make_pair<double, const char *>(1e6, "µ"),
            std::make_pair<double, const char *>(1e3, "m"),
            std::make_pair<double, const char *>(1, ""),
            std::make_pair<double, const char *>(1e-3, "k"),
            std::make_pair<double, const char *>(1e-6, "M"),
            std::make_pair<double, const char *>(1e-9, "G"),
        };
        static constexpr const double value_range[si_units.size() + 1] = {
            std::numeric_limits<double>::min(),
            1e-6,                                //
            1e-3,                                //
            1,                                   //
            1e3,                                 //
            1e6,                                 //
            1e9,                                 //
            std::numeric_limits<double>::max(),  //
        };
        // binary search
        unsigned short left = 0, right = si_units.size() - 1;
        unsigned short m = 0;
        while (left <= right)
        {
            m = (left + right) / 2;
            if (value_range[m] > value)
            {
                right = m - 1;
            }
            else if (value_range[m + 1] <= value)
            {
                left = m + 1;
            }
            else
            {
                break;
            }
        }
        return si_units[m];
    }

    inline std::pair<double, std::string>
    _get_time_factor_and_unit(double elapsed, bool fix_width = false)
    {
        auto &si_unit = get_si_unit_and_factor(elapsed);

        //        double factor = 1 / si_unit.first;
        std::string unit{si_unit.second};
        if (fix_width && unit.size() < 1)
            unit = " ";

        return {si_unit.first, unit + "s"};
    }

#define _FIX_WIDTH_DECIMAL(precision) /* +1 is for the decimal point */                            \
    std::setprecision((precision)) << std::left << std::setw((precision) + 1)

    inline std::string format_time2readable(const double elapsed, const int precision = 3)
    {
        const auto factor_and_unit = _get_time_factor_and_unit(elapsed, true);
        std::stringstream ss;
        ss << _FIX_WIDTH_DECIMAL(precision) << (elapsed * factor_and_unit.first)
           << factor_and_unit.second;
        return ss.str();
    }

    inline std::string
    format_time2readable(const std::pair<double, double> &mean_stdev, const int precision = 3)
    {
        // use mean to get factor and units
        const auto factor_and_unit = _get_time_factor_and_unit(mean_stdev.first, true);
        std::stringstream ss;
        ss << _FIX_WIDTH_DECIMAL(precision) << (mean_stdev.first * factor_and_unit.first) << "±"
           << _FIX_WIDTH_DECIMAL(precision) << (mean_stdev.second * factor_and_unit.first)
           << factor_and_unit.second;
        return ss.str();
    }

    inline std::string
    format_time2readable(const std::vector<double> &all_elapsed, const int precision = 3)
    {
        return format_time2readable(compute_mean_and_stdev(all_elapsed), precision);
    }

}  // namespace sxs

#endif  // SXS_FORMAT_H

#ifdef SXS_RUN_TESTS
/*
 * -------------------------------------------
 * Test cases and general usage for this file:
 * -------------------------------------------
 */

TEST_CASE("[sxs] Test format")
{
    using namespace sxs;

    SUBCASE("test si_unit")
    {
        auto invert_approx = [](double value) { return doctest::Approx(1 / value); };

        auto result = get_si_unit_and_factor(1);
        CHECK(result.first == invert_approx(1));
        CHECK_EQ(result.second, "");

        result = get_si_unit_and_factor(999);
        CHECK(result.first == invert_approx(1));
        CHECK_EQ(result.second, "");

        result = get_si_unit_and_factor(1000);
        CHECK(result.first == invert_approx(1e3));

        result = get_si_unit_and_factor(1001);
        CHECK(result.first == invert_approx(1e3));

        result = get_si_unit_and_factor(1125);
        CHECK(result.first == invert_approx(1e3));
        CHECK_EQ(result.second, "k");

        result = get_si_unit_and_factor(1e6 - 1);
        CHECK(result.first == invert_approx(1e3));

        result = get_si_unit_and_factor(1e6);
        CHECK(result.first == invert_approx(1e6));

        result = get_si_unit_and_factor(2.5e6);
        CHECK(result.first == invert_approx(1e6));
        CHECK_EQ(result.second, "M");

        result = get_si_unit_and_factor(2.5e9);
        CHECK(result.first == invert_approx(1e9));
        CHECK_EQ(result.second, "G");

        result = get_si_unit_and_factor(2.5e59);
        CHECK(result.first == invert_approx(1e9));
        CHECK_EQ(result.second, "G");

        result = get_si_unit_and_factor(2.5e-59);
        CHECK(result.first == invert_approx(1e-9));
        CHECK_EQ(result.second, "n");

        result = get_si_unit_and_factor(200e-9);
        CHECK(result.first == invert_approx(1e-9));
        CHECK_EQ(result.second, "n");

        result = get_si_unit_and_factor(25e-8);
        CHECK(result.first == invert_approx(1e-9));
        CHECK_EQ(result.second, "n");

        result = get_si_unit_and_factor(250e-6);
        CHECK(result.first == invert_approx(1e-6));
        CHECK_EQ(result.second, "µ");

        result = get_si_unit_and_factor(2500e-6);
        CHECK(result.first == invert_approx(1e-3));
        CHECK_EQ(result.second, "m");

        result = get_si_unit_and_factor(2.5e-4);
        CHECK(result.first == invert_approx(1e-6));
        CHECK_EQ(result.second, "µ");

        result = get_si_unit_and_factor(2e-3);
        CHECK(result.first == invert_approx(1e-3));
        CHECK_EQ(result.second, "m");

        result = get_si_unit_and_factor(0.1);
        CHECK(result.first == invert_approx(1e-3));

        result = get_si_unit_and_factor(0.001);
        CHECK(result.first == invert_approx(1e-3));

        result = get_si_unit_and_factor(0.001 - 0.00000001);
        CHECK(result.first == invert_approx(1e-6));

        result = get_si_unit_and_factor(1e-4);
        CHECK(result.first == invert_approx(1e-6));
    }

    SUBCASE("test get_time_unit")
    {
        auto result = _get_time_factor_and_unit(1);
        CHECK(result.first == doctest::Approx(1));
        CHECK_EQ(result.second, "s");

        result = _get_time_factor_and_unit(2e-3);
        CHECK(result.first == doctest::Approx(1e3));
        CHECK_EQ(result.second, "ms");

        result = _get_time_factor_and_unit(2e-6);
        CHECK(result.first == doctest::Approx(1e6));
        CHECK_EQ(result.second, "µs");

        result = _get_time_factor_and_unit(200e-6);
        CHECK(result.first == doctest::Approx(1e6));
        CHECK_EQ(result.second, "µs");

        result = _get_time_factor_and_unit(1000e-6);
        CHECK(result.first == doctest::Approx(1e3));
        CHECK_EQ(result.second, "ms");
    }

    // sxs::get_print_output_stream() = _original_ostream;
}

#endif  // SXS_RUN_TESTS