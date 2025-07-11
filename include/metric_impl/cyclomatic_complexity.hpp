#pragma once
#include <unistd.h>

#include <string>

#include "metric.hpp"

namespace analyser::metric::metric_impl {

struct CyclomaticComplexityMetric : IMetric {
  virtual ~CyclomaticComplexityMetric() = default;
  static std::string StaticName();

protected:
  MetricResult::ValueType CalculateImpl(const function::Function &f) const;
  std::string Name() const override;
};

} // namespace analyser::metric::metric_impl
