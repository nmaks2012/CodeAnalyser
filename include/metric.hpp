#pragma once
#include <algorithm>
#include <ranges>
#include <unistd.h>

#include <string>
#include <variant>
#include <vector>

#include "function.hpp"

namespace fs = std::filesystem;
namespace rv = std::ranges::views;
namespace rs = std::ranges;

namespace analyser::metric {

struct MetricResult {
  // using ValueType = int;
  using ValueType = std::variant<int, std::string>;
  std::string metric_name; // Название метрики
  ValueType value;         // Значение метрики
};

struct IMetric {
  virtual ~IMetric() = default;
  MetricResult Calculate(const function::Function &f) const {
    return MetricResult{.metric_name = Name(), .value = CalculateImpl(f)};
  }

protected:
  virtual MetricResult::ValueType
  CalculateImpl(const function::Function &f) const = 0;
  virtual std::string Name() const = 0;
};

using MetricResults = std::vector<MetricResult>;

struct MetricExtractor {
  void RegisterMetric(std::unique_ptr<IMetric> metric);

  MetricResults Get(const function::Function &func) const;
  std::vector<std::unique_ptr<IMetric>> metrics;
};

} // namespace analyser::metric

template <>
struct std::formatter<analyser::metric::MetricResults>
    : std::formatter<std::string> {
  template <typename FormatContext>
  auto format(const analyser::metric::MetricResults &results,
              FormatContext &ctx) const {

    rs::for_each(results | rv::enumerate, [&](const auto &p) {
      auto item = std::get<1>(p);
      auto idx = std::get<0>(p);
      if (holds_alternative<std::string>(item.value)) {

        std::format_to(ctx.out(), "{}: {}", item.metric_name,
                       std::get<std::string>(item.value));

      } else if (holds_alternative<int>(item.value)) {

        std::format_to(ctx.out(), "{}: {}", item.metric_name,
                       std::get<int>(item.value));

      } else {
        std::format_to(ctx.out(), "{}: {}", item.metric_name, "None");
      }
      if(idx < results.size() - 1){
        std::format_to(ctx.out(), "\n\t");
      } else {
        std::format_to(ctx.out(), "\n\n");
      }
    });

    return ctx.out();
  }
  
};