#pragma once
#include <algorithm>
#include <format>
#include <memory>

#include <string>
#include <unordered_map>
#include <vector>

#include "metric.hpp"
#include "metric_accumulator_impl/accumulators.hpp"

namespace rv = std::ranges::views;
namespace rs = std::ranges;

namespace analyser::metric_accumulator {

using namespace metric_accumulator_impl;

struct IAccumulator {
  virtual void Accumulate(const metric::MetricResult &metric_result) = 0;
  virtual void Finalize() = 0;
  virtual void Reset() = 0;
  virtual ~IAccumulator() = default;

protected:
  bool is_finalized = false;
};

struct MetricsAccumulator {

  using AccumulatorsContainer =
      std::unordered_map<std::string, std::shared_ptr<IAccumulator>>;

  template <typename Accumulator>
  void RegisterAccumulator(const std::string &metric_name,
                           std::unique_ptr<Accumulator> acc) {
    auto [it, inserted] = accumulators.try_emplace(metric_name, std::move(acc));
    if (!inserted) {
      throw CustomException(std::format("Accumulator {} for metric name {} is "
                                        "already registered",
                                        typeid(Accumulator).name(),
                                        metric_name));
    }
  }
  template <typename Accumulator>
  const Accumulator &
  GetFinalizedAccumulator(const std::string &metric_name) const {
    auto base_it = accumulators.find(metric_name);
    if (base_it == accumulators.end()) {
      throw CustomException(std::format("Accumulator {} for metric name {} is "
                                        "not registered",
                                        typeid(Accumulator).name(),
                                        metric_name));
    }

    Accumulator *accum_ptr = dynamic_cast<Accumulator *>(base_it->second.get());
    if (!accum_ptr) {
      throw CustomException(
          std::format("Stored accumulator for metric '{}' is not of the "
                      "requested type. Expected '{}'.",
                      metric_name, typeid(Accumulator).name()));
    }
    accum_ptr->Finalize();
    return *accum_ptr;
  }
  void AccumulateNextFunctionResults(
      const std::vector<metric::MetricResult> &metric_results) const;

  void ResetAccumulators();

  const AccumulatorsContainer &GetAccumulators() const { return accumulators; }

private:
  AccumulatorsContainer accumulators;
};

} // namespace analyser::metric_accumulator
