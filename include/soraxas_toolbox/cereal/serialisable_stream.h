//
// Created by tin on 22/07/22.
//

#ifndef SXS_CEREAL_SERIALISABLE_STREAM_H
#define SXS_CEREAL_SERIALISABLE_STREAM_H

#include <iostream>
#include <sstream>
#include <string>

namespace sxs {

/**
 * A serialisable stream object that wraps osteram and istream
 */
class SerialisableBinaryStream {
public:
  SerialisableBinaryStream() : initialised_(false) {}

  std::ostringstream &get_ostream() {
    ostream_ =
        std::ostringstream(std::stringstream::out | std::stringstream::binary);
    return ostream_;
  }

  std::istringstream &get_istream() {
    assert(initialised_);
    istream_ =
        std::istringstream(internal_string_buffer_,
                           std::stringstream::in | std::stringstream::binary);
    return istream_;
  }

  /**
   * Load the saved string back to a stream constructed as an istream
   * @tparam Archive
   * @param ar
   */
  template <class Archive> void load(Archive &ar) {
    ar(internal_string_buffer_);
    initialised_ = true;
  }

  /**
   * Save the stored stream from ostream
   * @tparam Archive
   * @param ar
   */
  template <class Archive> void save(Archive &ar) const {
    std::string out_str = ostream_.str();
    ar(out_str);
  }

private:
  std::ostringstream ostream_;
  std::istringstream istream_;
  std::string internal_string_buffer_;
  bool initialised_;
};
} // namespace sxs

#endif // SXS_CEREAL_SERIALISABLE_STREAM_H
