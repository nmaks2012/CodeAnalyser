#include "metric.hpp"

#include <ranges>
#include <unistd.h>

#include <vector>

#include "function.hpp"

namespace analyser::metric {

void MetricExtractor::RegisterMetric(std::unique_ptr<IMetric> metric) {
  metrics.push_back(std::move(metric));
}

MetricResults MetricExtractor::Get(const function::Function &func) const {
  return metrics | rv::transform([&](const auto &metric) {
           return metric->Calculate(func);
         }) |
         rs::to<MetricResults>();
}

} // namespace analyser::metric
