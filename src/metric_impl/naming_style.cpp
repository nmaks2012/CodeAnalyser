#include "metric_impl/naming_style.hpp"
#include "metric.hpp"

#include <algorithm>
#include <cctype>
#include <unistd.h>

#include <string>

namespace analyser::metric::metric_impl {

using namespace std::literals;

MetricResult::ValueType
NamingStyleMetric::CalculateImpl(const function::Function &f) const {

  // недопустимый первый символ - "_" или "число"
  bool unacceptable_first_symbol =
      std::isdigit(*f.name.begin()) || *f.name.begin() == '_';

  // два "__" подряд
  bool two_underscores = rs::adjacent_find(f.name, [](char a, char b) {
                           return a == '_' && b == '_';
                         }) != f.name.end();

  // Две заглавные буквы подряд
  bool two_uppercase = rs::adjacent_find(f.name, [](char a, char b) {
                         return std::isupper(a) && std::isupper(b);
                       }) != f.name.end();

  // Первый символ заглавная
  bool first_is_upper = std::isupper(*f.name.begin());

  // Есть заглавные
  bool exists_upper =
      rs::any_of(f.name, [](char c) { return std::isupper(c); });

  // Есть нижние подчеркивания
  bool exists_underscores =
      rs::any_of(f.name, [](char c) { return c == '_'; });

  // сразу отсекаем неизвестные случаи
  if (unacceptable_first_symbol || two_underscores || two_uppercase) {
    return "UNKNOWN"s;
  } else if (!exists_upper && !exists_underscores) {
    return "LOWERCASE"s;
  } else if (first_is_upper && !exists_underscores) {
    return "PASCALCASE"s;
  } else if (!first_is_upper && exists_upper && !exists_underscores) {
    return "CAMELCASE"s;
  } else if (!exists_upper && exists_underscores) {
    return "SNAKECASE"s;
  } else {
    return "UNKNOWN"s;
  }
};

std::string NamingStyleMetric::Name() const { return "NamingStyleMetric"; };

} // namespace analyser::metric::metric_impl
