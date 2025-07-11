#include "metric_impl/cyclomatic_complexity.hpp"
#include "metric.hpp"

#include <algorithm>
#include <array>
#include <flat_set>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <unistd.h>
#include <vector>

namespace analyser::metric::metric_impl {

using namespace std::literals;

MetricResult::ValueType
CyclomaticComplexityMetric::CalculateImpl(const function::Function &f) const {

  std::array<std::string_view, 11> CyclomaticOperators{
      "if_statement"sv,    "elif_clause"sv,     "else_clause"sv,
      "while_statement"sv, "for_statement"sv,   "try_statement"sv,
      "except_clause"sv,   "finally_clause"sv,  "match_statement"sv,
      "case_clause"sv,     "assert_statement"sv};
  // clang-format off
  auto operators_count = f.ast | 
        // Разбиваем AST строку на ноды
        rv::lazy_split('\n') |
        // Проверяем наличие цикломатических операторов в каждой ноде
        rv::filter([&](const auto &node) {
            return rs::any_of(CyclomaticOperators, [&](const auto &op) {
                return rs::contains_subrange(node, op);
            });
        });
  // clang-format on

  int complexity = std::ranges::distance(operators_count) + 1;

  return complexity;
}

std::string CyclomaticComplexityMetric::StaticName() {
  return "CyclomaticComplexityMetric";
};

std::string CyclomaticComplexityMetric::Name() const { return StaticName(); };

} // namespace analyser::metric::metric_impl
