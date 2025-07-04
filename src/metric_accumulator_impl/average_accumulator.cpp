#include "metric_accumulator_impl/average_accumulator.hpp"
#include "metric.hpp"
#include "metric_accumulator_impl/accumulators.hpp"

#include <unistd.h>
#include <variant>

namespace analyser::metric_accumulator::metric_accumulator_impl {

using namespace std::literals;

void AverageAccumulator::Accumulate(const metric::MetricResult &metric_result) {
  if (is_finalized) {
    throw CUSTOM_EXCEPTION("AverageAccumulator has finalized!");
  }
  if (std::holds_alternative<int>(metric_result.value)) {
    sum += std::get<int>(metric_result.value);
    count++;
  }
}

void AverageAccumulator::Finalize() {
  if (count > 0 && !is_finalized) {
    average = static_cast<double>(sum) / count;
  }
  is_finalized = true;
}

void AverageAccumulator::Reset() {
  sum = 0;
  count = 0;
  average = 0.0;
  is_finalized = false;
}

double AverageAccumulator::Get() const {
  if (!is_finalized) {
    throw CUSTOM_EXCEPTION("AverageAccumulator not finalize!");
  };
  return average;
}

} // namespace analyser::metric_accumulator::metric_accumulator_impl
