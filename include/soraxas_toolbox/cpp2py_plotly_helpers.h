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

// this requires headers from cpp2py library

#include "cpp2py_plotly.hpp"
#include "cpp2py_plotly_template.hpp"
#include "main.h"

void stats_aggregate_to_boxplot(
    Plotly::Figure &fig, const std::vector<std::pair<std::string, sxs::StatsAggregate>> &all_stats
)
{
    /*
     * Given a vector of pairs of <name,StatsAggregate>, plot them into a box
     * plot
     */
    // assume all StatsAggregate within all_stats contains the same set of name
    assert(all_stats.size() > 0);
    // TODO: test they have same set of name
    // extract stats names from data
    std::vector<std::string> stats_names;
    std::for_each(
        all_stats[0].second.data.begin(), all_stats[0].second.data.end(),
        [&stats_names](auto &&item) { stats_names.push_back(item.first); }
    );

    // create the container to pass to plotly template
    std::vector<Plotly::FigureTemplate::boxplot_datatype<double>> container;

    // extract stats value
    for (auto &&[stats_instance, stats] : all_stats)
    {
        std::vector<std::string> label;
        std::vector<double> y;

        // add all stats of this instance
        for (const std::string &stats_name : stats_names)
        {
            auto iter = stats.data.find(stats_name);
            if (iter == stats.data.end())
                throw std::runtime_error("Given StatsAggregate vector contains "
                                         "inconsistent stats.");
            auto stats_vals = (*iter).second;
            sxs::vec::extend(y, stats_vals);
            label.insert(label.end(), stats_vals.size(), stats_name);
        }
        container.emplace_back(stats_instance, label, y);
    }

    return Plotly::FigureTemplate::boxplot(fig, container);
}
