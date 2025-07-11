#include "metric_accumulator_impl/sum_average_accumulator.hpp"
#include "metric_accumulator_impl/accumulators.hpp"

#include <unistd.h>

namespace analyser::metric_accumulator::metric_accumulator_impl {

using namespace std::literals;

void SumAverageAccumulator::Accumulate(
    const metric::MetricResult &metric_result) {
  if (is_finalized) {
    throw CustomException("SumAverageAccumulator has finalized!");
  }
  if (std::holds_alternative<int>(metric_result.value)) {
    sum += std::get<int>(metric_result.value);
    count++;
  }
}

void SumAverageAccumulator::Finalize() {
  if (count > 0 && !is_finalized) {
    average = static_cast<double>(sum) / count;
  }
  is_finalized = true;
}

void SumAverageAccumulator::Reset() {
  sum = 0;
  count = 0;
  average = 0.0;
  is_finalized = false;
}

SumAverageAccumulator::SumAverage SumAverageAccumulator::Get() const {
  if (!is_finalized) {
    throw CustomException("SumAverageAccumulator not finalize!");
  };
  return {sum, average};
}

} // namespace analyser::metric_accumulator::metric_accumulator_impl
