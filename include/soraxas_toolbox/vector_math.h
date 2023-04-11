#pragma once

#include <numeric>
#include <vector>

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
}  // namespace sxs