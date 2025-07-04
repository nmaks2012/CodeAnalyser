#pragma once
#include <unistd.h>

#include <string>


#include "metric_accumulator.hpp"

namespace analyser::metric_accumulator::metric_accumulator_impl {

struct CategoricalAccumulator : public IAccumulator {

  using Container = std::unordered_map<std::string, int>;
  
  void Accumulate(const metric::MetricResult &metric_result) override;

  void Finalize() override;

  void Reset() override;

  const Container &Get() const;

private:
  Container categories_freq;
};

} // namespace analyser::metric_accumulator::metric_accumulator_impl
