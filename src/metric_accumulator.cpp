#include "metric_accumulator.hpp"
#include "metric.hpp"

#include <algorithm>
#include <unistd.h>

#include <vector>

namespace analyser::metric_accumulator {

void MetricsAccumulator::AccumulateNextFunctionResults(
    const std::vector<metric::MetricResult> &metric_results) const {
  rs::for_each(metric_results, [&](const auto &result) {
    auto It = accumulators.find(result.metric_name);
    if (It == accumulators.end()) {
      throw CustomException(std::format("Accumulator for metric '{}' is not "
                                        "registered",
                                        result.metric_name));
    }
    It->second->Accumulate(result);
  });
}

void MetricsAccumulator::ResetAccumulators() {
  rs::for_each(accumulators, [](auto &accum) { accum.second->Reset(); });
}

} // namespace analyser::metric_accumulator
