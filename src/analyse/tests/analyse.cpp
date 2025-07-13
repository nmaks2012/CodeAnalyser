#include <algorithm>
#include <cstddef>
#include <filesystem>
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
  const std::filesystem::path test_data_dir = TEST_DATA_DIR;
  std::vector<std::string> files{
      (test_data_dir / "advanced_processor.py").string(),
      (test_data_dir / "advanced_processor_func.py").string(),
      (test_data_dir / "advanced_processor_1.py").string()};
  void FillAllData() {
    // Метрики
    metric_extractor.RegisterMetric(std::make_unique<CodeLinesCountMetric>());
    metric_extractor.RegisterMetric(std::make_unique<CountParametersMetric>());
    metric_extractor.RegisterMetric(
        std::make_unique<CyclomaticComplexityMetric>());
    metric_extractor.RegisterMetric(std::make_unique<NamingStyleMetric>());
    // Аккумуляторы
    metric_accumulator.RegisterAccumulator(
        CyclomaticComplexityMetric::StaticName(),
        std::make_unique<SumAverageAccumulator>());

    metric_accumulator.RegisterAccumulator(
        CodeLinesCountMetric::StaticName(),
        std::make_unique<SumAverageAccumulator>());

    metric_accumulator.RegisterAccumulator(
        CountParametersMetric::StaticName(),
        std::make_unique<AverageAccumulator>());

    metric_accumulator.RegisterAccumulator(
        NamingStyleMetric::StaticName(),
        std::make_unique<CategoricalAccumulator>());
    // Анализ
    analysis = AnalyseFunctions(files, metric_extractor);
  };
  MetricExtractor metric_extractor;
  std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
  MetricsAccumulator metric_accumulator;
};

TEST_F(TestAnalyse, AnalyseFunctions) {
  // Arrange: Регистрируем все метрики и аккумуляторы, затем парсим тестовые
  // файлы.
  FillAllData();
  // Act: Анализ выполняется внутри FillAllData().
  // Assert: Проверяем, что было найдено и проанализировано ожидаемое количество
  // функций.
  EXPECT_TRUE(analysis.size() == 11);
}

TEST_F(TestAnalyse, SplitByClasses) {
  // Arrange: Регистрируем все метрики и аккумуляторы, затем парсим тестовые
  // файлы.
  FillAllData();
  EXPECT_EQ(analysis.size(), 11);
  // Act: Группируем результаты анализа по имени класса.
  auto split_reslut = SplitByClasses(analysis);
  // Assert: Проверяем, что результаты сгруппированы в правильное количество
  // классов.
  EXPECT_EQ(split_reslut.size(), 3);
}

TEST_F(TestAnalyse, SplitByFiles) {
  // Arrange: Регистрируем все метрики и аккумуляторы, затем парсим тестовые
  // файлы.
  FillAllData();
  EXPECT_EQ(analysis.size(), 11);
  // Act: Группируем результаты анализа по имени файла.
  auto split_reslut = SplitByFiles(analysis);
  // Assert: Проверяем, что результаты сгруппированы в правильное количество
  // файлов.
  EXPECT_EQ(split_reslut.size(), 3);
}

TEST_F(TestAnalyse, AverageAccumulator) {
  // Arrange: Регистрируем конкретную метрику и соответствующий аккумулятор.
  metric_extractor.RegisterMetric(std::make_unique<CodeLinesCountMetric>());
  metric_accumulator.RegisterAccumulator(
      CodeLinesCountMetric::StaticName(),
      std::make_unique<AverageAccumulator>());

  // Act: Запускаем анализ и накапливаем результаты.
  analysis = AnalyseFunctions(files, metric_extractor);
  rs::for_each(analysis, [&](const auto &item) {
    metric_accumulator.AccumulateNextFunctionResults(item.second);
  });

  // Assert: Проверяем, что итоговое среднее значение рассчитано корректно.
  auto accum = metric_accumulator.GetFinalizedAccumulator<AverageAccumulator>(
      CodeLinesCountMetric::StaticName());
  EXPECT_DOUBLE_EQ(accum.Get(), 2.5454545454545454);
}

TEST_F(TestAnalyse, CategoricalAccumulator) {
  // Arrange: Регистрируем конкретную метрику и соответствующий аккумулятор.
  metric_extractor.RegisterMetric(std::make_unique<NamingStyleMetric>());

  metric_accumulator.RegisterAccumulator(
      NamingStyleMetric::StaticName(),
      std::make_unique<CategoricalAccumulator>());

  // Act: Запускаем анализ и накапливаем результаты.
  analysis = AnalyseFunctions(files, metric_extractor);
  rs::for_each(analysis, [&](const auto &item) {
    metric_accumulator.AccumulateNextFunctionResults(item.second);
  });

  // Assert: Проверяем, что категориальные значения накоплены корректно.
  auto accum =
      metric_accumulator.GetFinalizedAccumulator<CategoricalAccumulator>(
          NamingStyleMetric::StaticName());

  std::unordered_map<std::string, int> expected{
      {"UNKNOWN", 5}, {"SNAKECASE", 3}, {"LOWERCASE", 3}};

  EXPECT_EQ(expected, accum.Get());
}

TEST_F(TestAnalyse, SumAverageAccumulator) {
  // Arrange: Регистрируем конкретную метрику и соответствующий аккумулятор.
  metric_extractor.RegisterMetric(std::make_unique<CodeLinesCountMetric>());

  metric_accumulator.RegisterAccumulator(
      CodeLinesCountMetric::StaticName(),
      std::make_unique<SumAverageAccumulator>());

  // Act: Запускаем анализ и накапливаем результаты.
  analysis = AnalyseFunctions(files, metric_extractor);
  rs::for_each(analysis, [&](const auto &item) {
    metric_accumulator.AccumulateNextFunctionResults(item.second);
  });

  // Assert: Проверяем, что сумма и среднее значение рассчитаны корректно.
  auto accum =
      metric_accumulator.GetFinalizedAccumulator<SumAverageAccumulator>(
          CodeLinesCountMetric::StaticName());
  EXPECT_EQ(accum.Get().sum, 28);
  EXPECT_DOUBLE_EQ(accum.Get().average, 2.5454545454545454);
}

TEST_F(TestAnalyse, GetUnregisteredAccumulatorThrows) {
  // Проверяет, что система выбросит исключение при попытке получить
  // аккумулятор, который не был зарегистрирован.
  EXPECT_THROW(metric_accumulator.GetFinalizedAccumulator<AverageAccumulator>(
                   "NonExistentAccumulator"),
               std::runtime_error);
}

TEST_F(TestAnalyse, GetAccumulatorWithWrongTypeThrows) {
  // Arrange: Регистрируем аккумулятор одного типа.
  const std::string name = "MyAccumulator";
  metric_accumulator.RegisterAccumulator(
      name, std::make_unique<AverageAccumulator>());

  // Act & Assert: Проверяем, что система выбросит исключение при попытке
  // получить аккумулятор с тем же именем, но другого типа.
  EXPECT_THROW(
      metric_accumulator.GetFinalizedAccumulator<CategoricalAccumulator>(name),
      std::runtime_error);
}

TEST_F(TestAnalyse, ResetAccumulators) {
  // Arrange: Регистрируем аккумулятор и накапливаем в нем начальные данные.
  const auto metric_name = CountParametersMetric::StaticName();
  metric_accumulator.RegisterAccumulator(
      metric_name, std::make_unique<AverageAccumulator>());
  metric_accumulator.AccumulateNextFunctionResults({{metric_name, 10}});
  metric_accumulator.AccumulateNextFunctionResults({{metric_name, 20}});

  // Act: Сбрасываем состояние аккумуляторов и накапливаем новые данные.
  metric_accumulator.ResetAccumulators();
  metric_accumulator.AccumulateNextFunctionResults({{metric_name, 3}});
  metric_accumulator.AccumulateNextFunctionResults({{metric_name, 5}});

  // Assert: Проверяем, что результат рассчитывается только по данным,
  // накопленным после сброса. (3 + 5) / 2 = 4.
  auto accum = metric_accumulator.GetFinalizedAccumulator<AverageAccumulator>(
      metric_name);
  EXPECT_DOUBLE_EQ(accum.Get(), 4.0);
}

TEST_F(TestAnalyse, AccumulatorWithNoData) {
  // Arrange: Регистрируем аккумулятор, но не передаем в него данные.
  const auto metric_name = CountParametersMetric::StaticName();
  metric_accumulator.RegisterAccumulator(
      metric_name, std::make_unique<AverageAccumulator>());
  // Act: Получаем финализированный аккумулятор.
  auto accum = metric_accumulator.GetFinalizedAccumulator<AverageAccumulator>(
      metric_name);
  // Assert: Проверяем, что аккумулятор возвращает 0, а не NaN или ошибку
  // деления на 0.
  EXPECT_DOUBLE_EQ(accum.Get(), 0.0);
}

} // namespace analyser