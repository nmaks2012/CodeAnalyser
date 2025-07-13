#include "metric_accumulator_impl/categorical_accumulator.hpp"
#include "metric_accumulator_impl/accumulators.hpp"

#include <string>
#include <unistd.h>
#include <variant>

namespace analyser::metric_accumulator::metric_accumulator_impl {

void CategoricalAccumulator::Accumulate(
    const metric::MetricResult &metric_result) {
  if (is_finalized) {
    throw CustomException("CategoricalAccumulator has finalized!");
  }

  if (std::holds_alternative<std::string>(metric_result.value)) {
    categories_freq[std::get<std::string>(metric_result.value)]++;
  }
};

void CategoricalAccumulator::Finalize() { is_finalized = true; };

void CategoricalAccumulator::Reset() {
  categories_freq.clear();
  is_finalized = false;
};

const CategoricalAccumulator::Container &CategoricalAccumulator::Get() const {
  if (!is_finalized) {
    throw CustomException("CategoricalAccumulator is not finalized!");
  }
  return categories_freq;
};

} // namespace analyser::metric_accumulator::metric_accumulator_impl
