#include "metric_impl/parameters_count.hpp"
#include "metric.hpp"

#include <algorithm>
#include <cctype>
#include <format>
#include <iterator>
#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

namespace analyser::metric::metric_impl {

using namespace std::literals;

MetricResult::ValueType
CountParametersMetric::CalculateImpl(const function::Function &f) const {

  // Определяем диапазон который относиться к параметрам
  auto start_search = rs::search(f.ast, "parameters:"s);
  auto end_search = rs::search(f.ast, "body:"s);
  // Если одна из границ не найдена, значит параметров нет
  if (start_search.empty() || end_search.empty()) {
    return 0;
  }

  // Находим конец строки с "parameters:", чтобы начать поиск со следующей.
  auto newline_it =
      rs::find(rs::subrange{start_search.begin(), f.ast.end()}, '\n');
  if (newline_it == f.ast.end()) {
    return 0; // Нет параметров на следующих строках
  }

  // Находим первый непробельный символ после этого \n.
  auto first_non_space_it =
      rs::find_if(rs::subrange{newline_it + 1, end_search.begin()}, [](char c) {
        return !std::isspace(static_cast<unsigned char>(c));
      });

  // Расстояние между ними - это и есть отступ.
  const auto indent = std::distance(newline_it + 1, first_non_space_it);

  // Параметры распологаются на расстоянии от начала строки
  // N пробельных символов и начинаются с оперделенных ключевых слов
  //  * identifier - неопределенный тип
  //  * default_parameter - параметр со значением по умолчанию
  //  * typed_parameter - типизированные параметры
  //  * typed_default_parameter - типизированные параметры со значением по
  //  умолчанию
  //  * list_splat_pattern - позиционные аргументы переменной длинны
  //  * dictionary_splat_pattern - словарь
  std::string pattern_string =
      std::format("\\n[\\s]{{{}}}" // Отступ от начала строки
                  "\\((?:identifier"
                  "|default_parameter"
                  "|typed_parameter"
                  "|typed_default_parameter"
                  "|list_splat_pattern"
                  "|dictionary_splat_pattern)",
                  indent);

  std::regex pattern_params(pattern_string);

  auto begin = std::sregex_iterator(start_search.begin(), end_search.begin(),
                                    pattern_params);

  auto end = std::sregex_iterator();

  int params_count = std::distance(begin, end);

  return params_count;
};

std::string CountParametersMetric::StaticName() {
  return "CountParametersMetric";
};

std::string CountParametersMetric::Name() const { return StaticName(); };

} // namespace analyser::metric::metric_impl
