#pragma once
#include <unistd.h>

#include <string>

#include "metric.hpp"

namespace analyser::metric::metric_impl {

struct NamingStyleMetric : public IMetric {

  virtual ~NamingStyleMetric() = default;
  static std::string StaticName();

protected:
  MetricResult::ValueType CalculateImpl(const function::Function &f) const;
  virtual std::string Name() const override;
};

} // namespace analyser::metric::metric_impl
