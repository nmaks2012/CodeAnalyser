#include "metric_impl/cyclomatic_complexity.hpp"
#include "metric.hpp"

#include <algorithm>
#include <flat_set>
#include <iterator>
#include <ranges>
#include <string>
#include <unistd.h>
#include <vector>

namespace analyser::metric::metric_impl {

using namespace std::literals;

MetricResult::ValueType
CyclomaticComplexityMetric::CalculateImpl(const function::Function &f) const {

  std::vector<std::string> CyclomaticOperators{
      "if_statement"s,    "elif_clause"s,     "else_clause"s,
      "while_statement"s, "for_statement"s,   "try_statement"s,
      "except_clause"s,   "finally_clause"s,  "match_statement"s,
      "case_clause"s,     "assert_statement"s};
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

  int complexity = std::ranges::distance(operators_count);

  return complexity;
}

std::string CyclomaticComplexityMetric::Name() const {
  return "CyclomaticComplexityMetric";
};

} // namespace analyser::metric::metric_impl
