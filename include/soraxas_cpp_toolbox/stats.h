//
// Created by soraxas on 4/18/21.
//

#ifndef SXS_STATS_H
#define SXS_STATS_H

#include "soraxas_cpp_toolbox/external/csv.hpp"
#include "soraxas_cpp_toolbox/main.h"

namespace sxs {

using stats_internal_variant = sxs::variant::variant<long, int, double, float>;
using stats_aggregate_internal_variant =
    sxs::variant::variant<std::vector<long>, std::vector<int>,
                          std::vector<double>, std::vector<float>>;

class Stats {
  /* A class that stores stats for performing basic performance test */
public:
  Stats(bool initialise_timer = true) {
    if (initialise_timer)
      m_timer = std::make_unique<sxs::Timer>();
  }

  // double is the default type
  template <
      class Numeric = double,
      typename = std::enable_if_t<std::is_arithmetic<Numeric>::value, Numeric>>
  Numeric &of(const std::string &key) {
    auto val_it = data.find(key);
    if (val_it != data.end()) {
      // return stored reference
      return sxs::variant::get<Numeric>(val_it.value());
    }

    // not found. Default to zero.
    data[key] = stats_internal_variant((Numeric)0); // cast to ensure correct
                                                    // type
    return sxs::variant::get<Numeric>(data[key]);
  }

  // return the actual std::variant
  stats_internal_variant &get(const std::string &key) { return data[key]; }

  // nicely format the contained items to the input stream
  template <typename T> void format_item(T &stream) const {
    // this works with anything that accepts << operator (and returns itself)
    bool firstitem = true;
    for (auto &&item : data) {
      if (firstitem)
        firstitem = false;
      else
        stream << ", ";
      stream << item.first << ": ";
      sxs::variant::visit([&stream](const auto &x) { stream << x; },
                          item.second);
    }
  }

  operator std::string() const {
    std::stringstream ss;
    ss << "{";
    format_item(ss);
    ss << "}";
    return ss.str();
  }

  friend std::ostream &operator<<(std::ostream &_stream, Stats const &t) {
    _stream << std::string(t);
    return _stream;
  }

  void reset() { data.clear(); }

  ~Stats() {
    if (!data.empty()) {
      std::cout << "========== Uncleared Stats ==========" << std::endl;
      std::cout << std::string(*this);
      std::cout << "=====================================" << std::endl;
    }
  }

  void set_stats_output_file(const std::string &filename,
                             bool write_header = true) {
    csv_output_file_stream = std::make_unique<std::ofstream>(filename);
    csv_output_file = std::make_unique<csv::CSVWriter<std::ofstream>>(
        *csv_output_file_stream);
    writer_stream_first_row_written = !write_header;
  }

  void serialise_to_csv(bool include_timestamp = true) {
    /* this function assumes no new stat type is added, and the order
     * reutrned by the map iterator maintains a stable order. */
    // write header row
    std::vector<std::string> cols;
    if (!writer_stream_first_row_written) {
      if (include_timestamp && m_timer)
        cols.push_back("timestamp");
      for (auto &&item : data) {
        cols.push_back(item.first);
      }
      writer_stream_first_row_written = true;
      csv_output_file->write_row(cols);
      cols.clear();
    }
    // data row
    if (include_timestamp && m_timer)
      cols.push_back(std::to_string(m_timer->elapsed()));
    for (auto &&item : data) {
      sxs::variant::visit(
          [&cols](const auto &x) { cols.push_back(std::to_string(x)); },
          item.second);
    }
    csv_output_file->write_row(cols);
  }

  std::unique_ptr<sxs::Timer> m_timer;
  std::unique_ptr<std::ofstream> csv_output_file_stream;
  std::unique_ptr<csv::CSVWriter<std::ofstream>> csv_output_file;
  bool writer_stream_first_row_written;

  tsl::ordered_map<std::string, stats_internal_variant> data;
};

class StatsAggregate {
  /* Given the Stats object, aggregate results into a std list */
public:
  StatsAggregate() = default;

  StatsAggregate(const Stats &stats) { append(stats); }

  void append(const Stats &stats) {
    for (auto &&item : stats.data) {
      // always push the item as a double (easier...)
      // this will always down-cast int/long/float within Stats to double
      sxs::variant::visit(
          [this, &item](const auto &x) { data[item.first].push_back(x); },
          item.second);
    }
  }

  operator std::string() const {
    bool firstitem = true;
    std::stringstream ss;
    ss << "{";
    for (auto &&item : data) {
      if (firstitem)
        firstitem = false;
      else
        ss << ", ";
      ss << item.first << ":[";
      for (size_t i = 0; i < item.second.size(); ++i) {
        if (i > 0)
          ss << ",";
        ss << item.second[i];
      }
      ss << "]";
    }
    ss << "}";
    return ss.str();
  }

  friend std::ostream &operator<<(std::ostream &_stream,
                                  StatsAggregate const &t) {
    _stream << std::string(t);
    return _stream;
  }

  const std::map<std::string, std::vector<double>> &get() const { return data; }

  std::map<std::string, std::vector<double>> data;
};

} // namespace sxs

#endif // SXS_STATS_H
