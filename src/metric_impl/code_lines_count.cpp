#include "metric_impl/code_lines_count.hpp"

#include <iterator>
#include <regex>
#include <string_view>
#include <unistd.h>

#include <algorithm>
#include <ranges>
#include <string>
#include <vector>

namespace analyser::metric::metric_impl {

using namespace std::literals;

MetricResult::ValueType
CodeLinesCountMetric::CalculateImpl(const function::Function &f) const {

  std::regex pattern_comment(
      "(expression_statement[\\W\\d\\n]+string|comment)"); // проверка вхождения
                                                           // комментариев """ и #

  std::regex pattern_function("function_definition");

  // clang-format off
  auto lines = f.ast | 
  // Разбиваем AST строку на ноды
  rv::split('\n') |  
  // группируем ноды AST по номерам строк исходного файла
  rv::chunk_by([](const auto& lhs, const auto& rhs){
    
    // получаем позиции номера строки для левого операнда
    auto begin_lhs = rs::find(lhs,'['); 
    auto end_lhs = rs::find(lhs, ',');
    // получаем позиции номера строки для правого операнда
    auto begin_rhs = rs::find(rhs,'[');
    auto end_rhs = rs::find(rhs, ',');
    // сравиваем
    return rs::equal(rs::subrange{begin_lhs, end_lhs}, rs::subrange{begin_rhs, end_rhs});
  
  }) | 
  // Преобразуем вложенные чанки к одной строке
  rv::transform([](const auto& chunk){
    return chunk | rv::join | rs::to<std::string>();
  }) |
  // Убираем строки с комментариями
  rv::filter([&](const auto& line){ 
    return !std::regex_search(line, pattern_comment) 
        && !std::regex_search(line, pattern_function);
  });

  return static_cast<int>(rs::distance(lines));
};

std::string CodeLinesCountMetric::Name() const {
  return "CodeLinesCountMetric";
};

} // namespace analyser::metric::metric_impl
