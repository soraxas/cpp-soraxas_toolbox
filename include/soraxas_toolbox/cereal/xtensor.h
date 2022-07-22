//
// Created by tin on 22/07/22.
//

#ifndef SXS_CEREAL_XTENSOR_H
#define SXS_CEREAL_XTENSOR_H

#include "soraxas_toolbox/cereal/serialisable_stream.h"
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <xtensor/xnpy.hpp>

namespace cereal {

// ==================================================
// xarray
// ==================================================

template <class Archive, typename T>
void save(Archive &archive, xt::xarray_container<T> const &m) {
  sxs::SerialisableBinaryStream sbs;
  xt::detail::dump_npy_stream(sbs.get_ostream(), m);
  archive(sbs);
}

template <class Archive, typename T>
void load(Archive &archive, xt::xarray_container<T> &m) {
  sxs::SerialisableBinaryStream sbs;
  archive(sbs);
  m = xt::load_npy<typename T::value_type>(sbs.get_istream());
}

// ==================================================
// xtensor
// ==================================================

template <class Archive, typename T, std::size_t N>
void save(Archive &archive, xt::xtensor_container<T, N> const &m) {
  sxs::SerialisableBinaryStream sbs;
  xt::detail::dump_npy_stream(sbs.get_ostream(), m);
  archive(sbs);
}

template <class Archive, typename T, std::size_t N>
void load(Archive &archive, xt::xtensor_container<T, N> &m) {
  sxs::SerialisableBinaryStream sbs;
  archive(sbs);
  m = xt::load_npy<typename T::value_type>(sbs.get_istream());
}

// ==================================================
// xfixed
// ==================================================

template <class Archive, typename T, typename N>
void save(Archive &archive, xt::xfixed_container<T, N> const &m) {
  sxs::SerialisableBinaryStream sbs;
  xt::detail::dump_npy_stream(sbs.get_ostream(), m);
  archive(sbs);
}

template <class Archive, typename T, typename N>
void load(Archive &archive, xt::xfixed_container<T, N> &m) {
  sxs::SerialisableBinaryStream sbs;
  archive(sbs);
  m = xt::load_npy<T>(sbs.get_istream());
}

} // namespace cereal

#endif // SXS_CEREAL_XTENSOR_H
