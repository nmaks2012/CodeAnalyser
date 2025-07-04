#pragma once

namespace analyser::metric_accumulator::metric_accumulator_impl {
#define CUSTOM_EXCEPTION(message)                                              \
  std::runtime_error(                                                            \
      std::format("{}\nin file: {},\nfunction: {},\nnumber_line: {}", message, \
                  __FILE__, __PRETTY_FUNCTION__, __LINE__));
} // namespace analyser::metric_accumulator::metric_accumulator_impl