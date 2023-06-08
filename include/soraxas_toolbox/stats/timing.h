#pragma once

#include "token.h"

#include <limits>
#include <math.h>

namespace sxs
{

    template <typename DataType = float>
    struct Stats
    {
        DataType min = std::numeric_limits<DataType>::max();
        DataType max = std::numeric_limits<DataType>::min();
        std::pair<DataType, DataType> mean_stdev = {0, std::numeric_limits<DataType>::quiet_NaN()};
        DataType sum = 0;
        size_t count = 0;

        const inline DataType &mean() const
        {
            return mean_stdev.first;
        }

        const inline DataType &stdev() const
        {
            return mean_stdev.second;
        }

        inline DataType &mean()
        {
            return mean_stdev.first;
        }

        inline DataType &stdev()
        {
            return mean_stdev.second;
        }

        void accumulate_standard(const Stats &rhs)
        {
            // combine stdev with
            // https://math.stackexchange.com/questions/2971315/how-do-i-combine-standard-deviations-of-two-groups
            stdev() =
                ((count - 1) * std::pow(stdev(), 2) + (rhs.count - 1) * std::pow(rhs.stdev(), 2)) /
                    (count + rhs.count - 1) +
                ((count * rhs.count) * std::pow(mean() - rhs.mean(), 2)) /
                    ((count + rhs.count) * (count + rhs.count - 1));
            mean() = mean() * count + rhs.mean() * rhs.count;

            min = std::min(min, rhs.min);
            max = std::max(max, rhs.max);
            count += rhs.count;
            sum += rhs.sum;
        }

        void accumulate_without_mean_stdev(const Stats &rhs)
        {
            min = std::min(min, rhs.min);
            max = std::max(max, rhs.max);
            count += rhs.count;
            sum += rhs.sum;
        }
    };
};  // namespace sxs
