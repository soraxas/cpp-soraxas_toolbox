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

//
// Created by tin on 22/07/22.
//

#ifndef SXS_CEREAL_SERIALISABLE_STREAM_H
#define SXS_CEREAL_SERIALISABLE_STREAM_H

#include <iostream>
#include <sstream>
#include <string>

namespace sxs
{

    /**
     * A serialisable stream object that wraps osteram and istream
     */
    class SerialisableBinaryStream
    {
    public:
        SerialisableBinaryStream() : initialised_(false)
        {
        }

        std::ostringstream &get_ostream()
        {
            ostream_ = std::ostringstream(std::stringstream::out | std::stringstream::binary);
            return ostream_;
        }

        std::istringstream &get_istream()
        {
            assert(initialised_);
            istream_ = std::istringstream(
                internal_string_buffer_, std::stringstream::in | std::stringstream::binary
            );
            return istream_;
        }

        /**
         * Load the saved string back to a stream constructed as an istream
         * @tparam Archive
         * @param ar
         */
        template <class Archive>
        void load(Archive &ar)
        {
            ar(internal_string_buffer_);
            initialised_ = true;
        }

        /**
         * Save the stored stream from ostream
         * @tparam Archive
         * @param ar
         */
        template <class Archive>
        void save(Archive &ar) const
        {
            std::string out_str = ostream_.str();
            ar(out_str);
        }

    private:
        std::ostringstream ostream_;
        std::istringstream istream_;
        std::string internal_string_buffer_;
        bool initialised_;
    };
}  // namespace sxs

#endif  // SXS_CEREAL_SERIALISABLE_STREAM_H
