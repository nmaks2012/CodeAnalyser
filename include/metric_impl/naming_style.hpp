#pragma once
#include <unistd.h>

#include <string>

#include "metric.hpp"

namespace analyser::metric::metric_impl {

struct NamingStyleMetric : public IMetric {

  virtual ~NamingStyleMetric() = default;

protected:
  MetricResult::ValueType CalculateImpl(const function::Function &f) const;
  virtual std::string Name() const;
};

} // namespace analyser::metric::metric_impl
