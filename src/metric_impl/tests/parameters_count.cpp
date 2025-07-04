#include "metric_impl/parameters_count.hpp"
#include "metric.hpp"

#include <gtest/gtest.h>

namespace analyser::metric::metric_impl {

struct TestParamsCP {
  std::string filename;
  int expected_result;
};

class CountParameters : public ::testing::TestWithParam<TestParamsCP> {
protected:
  void SetFile(const std::string &filename) {
    functions = extractor.Get({filename});
  }
  analyser::function::FunctionExtractor extractor;
  std::vector<function::Function> functions;
};

TEST_P(CountParameters, Test) {
  TestParamsCP params = GetParam();
  SetFile(params.filename);
  auto metric = CountParametersMetric();
  MetricResult::ValueType result = metric.Calculate(*functions.begin()).value;
  EXPECT_EQ(std::get<int>(result), params.expected_result);
}

INSTANTIATE_TEST_SUITE_P(Add, CountParameters,
                         testing::Values(TestParamsCP{"many_lines.py", 0},
                                         TestParamsCP{"comments.py", 3},
                                         TestParamsCP{"exceptions.py", 0},
                                         TestParamsCP{"if.py", 1},
                                         TestParamsCP{"loops.py", 1},
                                         TestParamsCP{"many_parameters.py", 5},
                                         TestParamsCP{"match_case.py", 1},
                                         TestParamsCP{"nested_if.py", 2},
                                         TestParamsCP{"simple.py", 0},
                                         TestParamsCP{"ternary.py", 1}));

} // namespace analyser::metric::metric_impl
