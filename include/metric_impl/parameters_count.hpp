#pragma once

#include <string>

#include "metric.hpp"

namespace analyser::metric::metric_impl {

struct CountParametersMetric final : public IMetric {
  virtual ~CountParametersMetric() = default;

protected:
  MetricResult::ValueType CalculateImpl(const function::Function &f) const;
  virtual std::string Name() const;
};

} // namespace analyser::metric::metric_impl
