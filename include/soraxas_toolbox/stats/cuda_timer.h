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

#include "../external/ordered-map/ordered_map.h"

#include <cuda_runtime.h>
#include <functional>
#include <iostream>

// #define USE_CUDA_CHRONO
#ifdef USE_CUDA_CHRONO
#include <cuda/std/chrono>
#endif

namespace sxs
{

template <class Token, size_t BufferSize>
class TimeStamperCUDA
{
    static_assert(
        BufferSize >= 2, "Must be at least of size 2 (1 will be used for begin, and 1 for "
                         "actual usage"
    );

public:
    using token_t = Token;
    using stat_t = TimeStamperStatIteratorReturnType<Token>;
    // clang-format off
        // conditionally use smaller size to store index_
        using StorageIndexType =
                std::conditional_t<
                        BufferSize < (2 << (8 - 1)), uint8_t,
                        std::conditional_t<
                                BufferSize < (2 << (16 - 1)), uint16_t,
                                std::conditional_t<
                                        BufferSize < (2 << (31 - 1)), uint32_t,
                                        uint_fast32_t
                                >>>;

    // clang-format on

    struct StampedData
    {
        Token event;
#ifdef USE_CUDA_CHRONO
        std::chrono::high_resolution_clock::time_point timepoint;
#else
        clock_t timepoint;
#endif
    };

    explicit TimeStamperCUDA() = default;

    ~TimeStamperCUDA() = default;

    __forceinline__ __device__ __host__ void reset()
    {
        index_ = 0;
    }

    template <Token e>
    __forceinline__ __device__ void stamp()
    {
        if (index_ < BufferSize)
        {
            events_[index_].event = e;
#ifdef USE_CUDA_CHRONO
            events_[index_].timepoint = std::chrono::high_resolution_clock::now();
#else
            events_[index_].timepoint = clock();
#endif

            ++index_;
        }
    }

    //    private:

    StampedData events_[BufferSize];
    StorageIndexType index_;

    /////////////////////////////////////////////
    /////////////////////////////////////////////

    // member typedefs provided through inheriting from std::iterator
    class iterator
    {
        const StampedData *events_data_ref_;
        long num;

    public:
        explicit iterator(const StampedData *ref, long _num) : events_data_ref_(ref), num(_num)
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
            //                double dur = ((double) (events_data_ref_[num + 1].timepoint -
            //                events_data_ref_[num].timepoint
            //                )) / CLOCKS_PER_SEC / CLOCKS_PER_SEC / CLOCKS_PER_SEC;
            //                 std::cout << dur << " " << events_data_ref_[num ].timepoint <<
            //                 std::endl;
            assert(num + 1 < index_);
            assert(events_data_ref_[num].timepoint < events_data_ref_[num + 1].timepoint);
            return std::make_tuple<Token, Token, double>(
                ((Token)events_data_ref_[num].event), ((Token)events_data_ref_[num + 1].event),
#ifdef USE_CUDA_CHRONO
                std::chrono::duration<double, std::ratio<1>>(
                    events_data_ref_[num + 1].timepoint - events_data_ref_[num].timepoint
                )
#else
                ((double)(events_data_ref_[num + 1].timepoint - events_data_ref_[num].timepoint)) /
                    CLOCKS_PER_SEC / CLOCKS_PER_SEC / CLOCKS_PER_SEC
#endif
            );
        }
    };

    iterator begin() const
    {
        return iterator(events_, 0);
    }

    iterator end() const
    {
        // FIXME currently this will leads to begin() == end() never be true if the container is
        // empty
        return iterator(events_, index_ - 1);
    }

    bool check_validity() const
    {
        if (index_ >= BufferSize - 1)
        {
            printf("================ WARNING ====================\n");
            printf(" Buffer is full capacity at %u/%u\n", index_, BufferSize);
            printf(" Some measurement might be missing.\n");
            printf("=============================================\n");
            return false;
        }
        return true;
    }
};
}  // namespace sxs
