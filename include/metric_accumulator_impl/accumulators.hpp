#pragma once

#include <format>
#include <source_location>
#include <stdexcept>
#include <string_view>

namespace analyser::metric_accumulator::metric_accumulator_impl {
inline std::runtime_error CustomException(
    std::string_view message,
    const std::source_location &location = std::source_location::current()) {
  return std::runtime_error(std::format(
      "{}\nin file: {},\nfunction: {},\nline: {}", message,
      location.file_name(), location.function_name(), location.line()));
}
} // namespace analyser::metric_accumulator::metric_accumulator_impl