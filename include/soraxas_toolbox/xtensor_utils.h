//
// Created by tin on 22/07/22.
//

#ifndef SXS_XTENSOR_UTILS_H
#define SXS_XTENSOR_UTILS_H

#include <xtensor/xfunction.hpp>

template <class F>
std::ostream &operator<<(std::ostream &out,
                         const typename xt::xfunction<F>::shape_type &shape) {
  out << "shape[";
  for (int i = 0; i < shape.size(); ++i) {
    out << shape.at(i);
    if (i < shape.size() - 1)
      out << ",";
  }
  out << "]";
  return out;
}



#endif
