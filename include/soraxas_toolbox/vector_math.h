#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

namespace sxs
{

    template <typename Container, typename DataType = typename Container::value_type>
    inline DataType compute_sum(Container nums)
    {
        return std::accumulate(nums.begin(), nums.end(), 0.);
    }

    /*
     * With pre-computed mean.
     */
    template <typename Container, typename FloatingPointDataType>
    inline FloatingPointDataType compute_stdev(FloatingPointDataType mean, Container nums)
    {
        if (nums.size() == 1)
            return 0;
        FloatingPointDataType accum = 0.0;
        std::for_each(
            std::begin(nums), std::end(nums), [&](auto &&d) { accum += (d - mean) * (d - mean); }
        );

        return sqrt(accum / (nums.size() - 1));
    }

    template <typename Container, typename FloatingPointDataType = double>
    inline std::pair<FloatingPointDataType, FloatingPointDataType>
    compute_mean_and_stdev(Container nums)
    {
        const auto sum = compute_sum(nums);
        const FloatingPointDataType mean = (FloatingPointDataType)sum / nums.size();

        FloatingPointDataType accum = 0.0;
        std::for_each(
            std::begin(nums), std::end(nums), [&](auto &&d) { accum += (d - mean) * (d - mean); }
        );

        FloatingPointDataType stdev = sqrt(accum / (nums.size() - 1));

        return {mean, stdev};
    }

}  // namespace sxs

#ifdef SXS_RUN_TESTS
/*
 * -------------------------------------------
 * Test cases and general usage for this file:
 * -------------------------------------------
 */

TEST_CASE("[sxs] An int vector with some items")
{
    std::vector<int> v = {1, 2, 3, 4, 5};

    CHECK(sxs::compute_sum(v) == 15);

    auto mean_stdev = sxs::compute_mean_and_stdev(v);
    CHECK(mean_stdev.first == (sxs::compute_sum(v) / v.size()));
    CHECK(mean_stdev.second == doctest::Approx(1.5811388301));
}

TEST_CASE("[sxs] A double vector with some items")
{
    std::vector<double> v = {1.5, 2.9, 33.4};

    CHECK(sxs::compute_sum(v) == 37.8);

    auto mean_stdev = sxs::compute_mean_and_stdev(v);
    CHECK(mean_stdev.first == (sxs::compute_sum(v) / v.size()));
    CHECK(mean_stdev.second == doctest::Approx(18.0269243078));

    SUBCASE("Computing stdev with pre-computed sum")
    {
        double my_sum = sxs::compute_sum(v);

        CHECK(mean_stdev.second == doctest::Approx(sxs::compute_stdev(my_sum / v.size(), v)));
    }
}

#endif  // SXS_RUN_TESTS
