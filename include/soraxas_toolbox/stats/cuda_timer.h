#pragma once

#include "timer_interface.h"

#include "../external/ordered-map/ordered_map.h"

#include <cuda_runtime.h>
#include <functional>

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
            BufferSize < (2<<(8-1)), uint8_t,
                std::conditional_t<
                BufferSize < (2<<(16-1)), uint16_t,
                        std::conditional_t<
                        BufferSize < (2<<(31-1)), uint32_t,
                            uint_fast32_t
                            >>>;

        // clang-format on

        struct StampedData
        {
            Token event;
            clock_t timepoint;
        };

        explicit TimeStamperCUDA() = default;

        ~TimeStamperCUDA() = default;

        template <Token e>
        __forceinline__ __device__ void stamp()
        {
            if (index_ < BufferSize)
            {
                events_[index_].event = e;
                events_[index_].timepoint = clock();

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
                return std::make_tuple<Token, Token, double>(
                    ((Token)events_data_ref_[num].event), ((Token)events_data_ref_[num + 1].event),
                    ((double)events_data_ref_[num + 1].timepoint - events_data_ref_[num].timepoint
                    ) / CLOCKS_PER_SEC
                );
            }
        };

        iterator begin()
        {
            return iterator(events_, 0);
        }

        iterator end()
        {
            if (index_ >= BufferSize - 1)
            {
                printf("================ WARNING ====================\n");
                printf(" Buffer is full capacity at %u/%u\n", index_, BufferSize);
                printf(" Some measurement might be missing.\n");
                printf("=============================================\n");
            }
            return iterator(events_, index_ - 1);
        }
    };
}  // namespace sxs
