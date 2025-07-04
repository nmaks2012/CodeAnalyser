#include <algorithm>
#include <memory>
#include <unistd.h>

#include <print>
#include <vector>

#include "analyse.hpp"
#include "cmd_options.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"
#include "metric_accumulator_formatter.hpp"
#include "metric_accumulator_impl/average_accumulator.hpp"
#include "metric_accumulator_impl/categorical_accumulator.hpp"
#include "metric_accumulator_impl/sum_average_accumulator.hpp"
#include "metric_impl/code_lines_count.hpp"
#include "metric_impl/cyclomatic_complexity.hpp"
#include "metric_impl/naming_style.hpp"
#include "metric_impl/parameters_count.hpp"

int main(int argc, char *argv[]) {
  analyser::cmd::ProgramOptions options;
  if (!options.Parse(argc, argv)) {
    return 0;
  }

  using namespace analyser::metric::metric_impl;
  using namespace analyser::metric;
  using namespace analyser::metric_accumulator::metric_accumulator_impl;

  MetricExtractor metric_extractor;
  metric_extractor.RegisterMetric(std::make_unique<CodeLinesCountMetric>());
  metric_extractor.RegisterMetric(
      std::make_unique<CyclomaticComplexityMetric>());
  metric_extractor.RegisterMetric(std::make_unique<NamingStyleMetric>());
  metric_extractor.RegisterMetric(std::make_unique<CountParametersMetric>());

  auto files = options.GetFiles();
  auto analyse = analyser::AnalyseFunctions(files, metric_extractor);

  std::println("{}", analyse);

  analyser::metric_accumulator::MetricsAccumulator accumulators;
  accumulators.RegisterAccumulator("Average",
                                   std::make_unique<AverageAccumulator>());
  accumulators.RegisterAccumulator("SumAverage",
                                   std::make_unique<SumAverageAccumulator>());
  accumulators.RegisterAccumulator("Categorical",
                                   std::make_unique<CategoricalAccumulator>());

  // запустите analyser::SplitByFiles
  rs::for_each(analyser::SplitByFiles(analyse), [&](const auto &file_analysis) {
    accumulators.ResetAccumulators();
    analyser::AccumulateFunctionAnalysis(file_analysis, accumulators);
    std::println("Accumulated Analysis for file {}\n\t{}",
                 file_analysis.front().first.filename, accumulators);
  });

  // запустите analyser::SplitByClasses
  rs::for_each(
      analyser::SplitByClasses(analyse), [&](const auto &class_analysis) {
        accumulators.ResetAccumulators();
        analyser::AccumulateFunctionAnalysis(class_analysis, accumulators);
        std::println("Accumulated Analysis for class {}\n\t{}",
                     class_analysis.front().first.class_name.value_or("N/A"),
                     accumulators);
      });

  // запустите analyser::AccumulateFunctionAnalysis для всех результатов метрик
  accumulators.ResetAccumulators();
  analyser::AccumulateFunctionAnalysis(analyse, accumulators);
  std::println("Accumulated Analysis for all functions:\n\t{}", accumulators);

  return 0;
}
