#pragma once

#include <algorithm>
#include <format>
#include <ranges>

#include <print>
#include <string>
#include <utility>
#include <vector>

#include "function.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"
#include "metric_impl/naming_style.hpp"

namespace analyser {

namespace rv = std::ranges::views;
namespace rs = std::ranges;

auto AnalyseFunctions(
    const std::vector<std::string> &files,
    const analyser::metric::MetricExtractor &metric_extractor) {

  function::FunctionExtractor fextractor;
  // clang-format off
  return files |
        // Создаем структуры File
         rv::transform([&](const auto &file) { return fextractor.Get(file); }) |
         // Объединяем результаты в один диапазон
         rv::join | 
         // Вычисляем метрики для каждой функции
         rv::transform([&](const auto &function) {
           return std::pair{function, metric_extractor.Get(function)};
         }) |
         // Преобразуем в итоговый контейнер
         rs::to<std::vector<std::pair<function::Function, metric::MetricResults>>>();
  // clang-format on
}

template <typename T> auto SplitByClasses(const T &analysis) {

  return analysis | rv::filter([](const auto &func) {
           return func.first.class_name.has_value();
         }) |
         rv::chunk_by([](const auto &a, const auto &b) {
           return a.first.class_name.value() == b.first.class_name.value();
         }) |
         rs::to<std::vector<T>>();
}

template <typename T> auto SplitByFiles(const T &analysis) {

  return analysis | rv::chunk_by([](const auto &a, const auto &b) {
           return a.first.filename == b.first.filename;
         }) |
         rs::to<std::vector<T>>();
}

void AccumulateFunctionAnalysis(
    const auto &analysis,
    const analyser::metric_accumulator::MetricsAccumulator &accumulator) {

  rs::for_each(analysis, [&](const auto &pair) {
    accumulator.AccumulateNextFunctionResults(pair.second);
  });
}

} // namespace analyser

template <>
struct std::formatter<std::vector<
    std::pair<analyser::function::Function, analyser::metric::MetricResults>>>
    : std::formatter<std::string> {
  template <typename FormatContext>
  auto format(const std::vector<std::pair<analyser::function::Function,
                                          analyser::metric::MetricResults>> &p,
              FormatContext &ctx) const {

    rs::for_each(p, [&](const auto &line) {
      std::format_to(ctx.out(), "{}[::{}]::{}:\n\t", line.first.filename,
                     line.first.class_name.has_value()
                         ? line.first.class_name.value()
                         : "None",
                     line.first.name, line.first.name);
      std::format_to(ctx.out(), "{}", line.second);
    });

    return ctx.out();
  }
};
