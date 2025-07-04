#include <algorithm>
#include <cstddef>
#include <flat_map>
#include <gtest/gtest.h>
#include <memory>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

#include "analyse.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"
#include "metric_accumulator_impl/average_accumulator.hpp"
#include "metric_accumulator_impl/categorical_accumulator.hpp"
#include "metric_accumulator_impl/sum_average_accumulator.hpp"
#include "metric_impl/code_lines_count.hpp"
#include "metric_impl/cyclomatic_complexity.hpp"
#include "metric_impl/naming_style.hpp"
#include "metric_impl/parameters_count.hpp"

namespace analyser {

using namespace metric;
using namespace metric::metric_impl;
using namespace std::literals;
using namespace metric_accumulator::metric_accumulator_impl;
using namespace analyser::metric;
using namespace metric_accumulator;

class TestAnalyse : public ::testing::Test {
protected:
  void SetUp() override {
    metric_extractor.RegisterMetric(std::make_unique<CodeLinesCountMetric>());
    metric_extractor.RegisterMetric(
        std::make_unique<CyclomaticComplexityMetric>());
    metric_extractor.RegisterMetric(std::make_unique<NamingStyleMetric>());
    metric_extractor.RegisterMetric(std::make_unique<CountParametersMetric>());
    analysis = AnalyseFunctions(files, metric_extractor);
  }

  std::vector<std::string> files{"advanced_processor.py",
                                 "advanced_processor_func.py",
                                 "advanced_processor_1.py"};
  MetricExtractor metric_extractor;
  std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
  MetricsAccumulator metric_accumulator;
};

TEST_F(TestAnalyse, AnalyseFunctions) { EXPECT_TRUE(analysis.size() == 11); }

TEST_F(TestAnalyse, SplitByClasses) {

  EXPECT_EQ(analysis.size(), 11);
  auto split_reslut = SplitByClasses(analysis);
  EXPECT_EQ(split_reslut.size(), 3);
}

TEST_F(TestAnalyse, SplitByFiles) {

  EXPECT_EQ(analysis.size(), 11);
  auto split_reslut = SplitByFiles(analysis);
  EXPECT_EQ(split_reslut.size(), 3);
}

TEST_F(TestAnalyse, AverageAccumulator) {

  std::string name{"Average"};
  metric_accumulator.RegisterAccumulator(
      name, std::make_unique<AverageAccumulator>());

  rs::for_each(analysis, [&](const auto &item) {
    metric_accumulator.AccumulateNextFunctionResults(item.second);
  });

  auto accum =
      metric_accumulator.GetFinalizedAccumulator<AverageAccumulator>(name);

  EXPECT_DOUBLE_EQ(accum.Get(), 1.393939393939394);
}

TEST_F(TestAnalyse, CategoricalAccumulator) {

  std::string name{"Categorical"};
  metric_accumulator.RegisterAccumulator(
      name, std::make_unique<CategoricalAccumulator>());

  rs::for_each(analysis, [&](const auto &item) {
    metric_accumulator.AccumulateNextFunctionResults(item.second);
  });

  auto accum =
      metric_accumulator.GetFinalizedAccumulator<CategoricalAccumulator>(name);

  std::unordered_map<std::string, int> expected{
      {"UNKNOWN", 5}, {"SNAKECASE", 3}, {"LOWERCASE", 3}};

  EXPECT_TRUE(rs::all_of(accum.Get(), [&](const auto &item) {
    return expected.contains(item.first) && item.second == expected[item.first];
  }));
}

TEST_F(TestAnalyse, SumAverageAccumulator) {

  std::string name{"SumAverage"};
  metric_accumulator.RegisterAccumulator(
      name, std::make_unique<SumAverageAccumulator>());

  rs::for_each(analysis, [&](const auto &item) {
    metric_accumulator.AccumulateNextFunctionResults(item.second);
  });

  auto accum =
      metric_accumulator.GetFinalizedAccumulator<SumAverageAccumulator>(name);

  EXPECT_EQ(accum.Get().sum, 46);
  EXPECT_DOUBLE_EQ(accum.Get().average, 1.393939393939394);
}

TEST_F(TestAnalyse, AllAccumulators) {

  std::string name_categorical{"Categorical"};
  std::string name_average{"Average"};
  std::string name_sum_average{"SumAverage"};

  metric_accumulator.RegisterAccumulator(
      name_categorical, std::make_unique<CategoricalAccumulator>());
  metric_accumulator.RegisterAccumulator(
      name_average, std::make_unique<AverageAccumulator>());
  metric_accumulator.RegisterAccumulator(
      name_sum_average, std::make_unique<SumAverageAccumulator>());

  rs::for_each(analysis, [&](const auto &item) {
    metric_accumulator.AccumulateNextFunctionResults(item.second);
  });

  auto categorical_accum =
      metric_accumulator.GetFinalizedAccumulator<CategoricalAccumulator>(
          name_categorical);
  std::unordered_map<std::string, int> expected{
      {"UNKNOWN", 5}, {"SNAKECASE", 3}, {"LOWERCASE", 3}};

  EXPECT_TRUE(rs::all_of(categorical_accum.Get(), [&](const auto &item) {
    return expected.contains(item.first) && item.second == expected[item.first];
  }));

  auto average_accum =
      metric_accumulator.GetFinalizedAccumulator<AverageAccumulator>(
          name_average);

  EXPECT_DOUBLE_EQ(average_accum.Get(), 1.393939393939394);

  auto sum_average_accum =
      metric_accumulator.GetFinalizedAccumulator<SumAverageAccumulator>(
          name_sum_average);

  EXPECT_EQ(sum_average_accum.Get().sum, 46);
  EXPECT_DOUBLE_EQ(sum_average_accum.Get().average, 1.393939393939394);
}

} // namespace analyser