#include "metric_accumulator.hpp"
#include "metric_accumulator_impl/accumulators.hpp"
#include "metric_accumulator_impl/average_accumulator.hpp"
#include "metric_accumulator_impl/categorical_accumulator.hpp"
#include "metric_accumulator_impl/sum_average_accumulator.hpp"
#include <algorithm>
#include <format>

using namespace analyser::metric_accumulator::metric_accumulator_impl;

template <>
struct std::formatter<analyser::metric_accumulator::MetricsAccumulator>
    : std::formatter<std::string> {
  template <typename FormatContext>
  auto format(const analyser::metric_accumulator::MetricsAccumulator &accums,
              FormatContext &ctx) const {

    rs::for_each(accums.GetAccumulators(), [&](const auto &accummulator_pair) {
      auto &[name, accum_ptr] = accummulator_pair;
      accum_ptr->Finalize();

      if (auto *avg_accum =
              dynamic_cast<AverageAccumulator *>(accum_ptr.get())) {

        std::format_to(ctx.out(), "{}: {:.2f}\n\t", name, avg_accum->Get());

      } else if (auto *sum_avg_accum =
                     dynamic_cast<SumAverageAccumulator *>(accum_ptr.get())) {

        auto result = sum_avg_accum->Get();
        std::format_to(ctx.out(), "{}: Sum = {}, Average = {:.2f}\n\t", name,
                       result.sum, result.average);

      } else if (auto *cat_accum =
                     dynamic_cast<CategoricalAccumulator *>(accum_ptr.get())) {

        auto const &categories = cat_accum->Get();
        std::format_to(ctx.out(), "{}: Categories = {{ ", name);
        if (!categories.empty()) {
          auto it = categories.cbegin();
          std::format_to(ctx.out(), "\"{}\": {}", it->first, it->second);
          rs::for_each(++it, categories.cend(), [&](const auto &pair) {
            std::format_to(ctx.out(), ", \"{}\": {}", pair.first, pair.second);
          });
        }
        std::format_to(ctx.out(), " }}\n\t");
      }
    });

    return ctx.out();
  }
};
