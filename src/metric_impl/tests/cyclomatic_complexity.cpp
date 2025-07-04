#include "metric_impl/cyclomatic_complexity.hpp"
#include "metric.hpp"

#include <gtest/gtest.h>

namespace analyser::metric::metric_impl {

struct TestParamsCC {
  std::string filename;
  int expected_result;
};

class CyclomaticComplexty : public ::testing::TestWithParam<TestParamsCC> {
protected:
  void SetFile(const std::string &filename) {
    functions = extractor.Get({filename});
  }
  analyser::function::FunctionExtractor extractor;
  std::vector<function::Function> functions;
};

TEST_P(CyclomaticComplexty, Test) {
  TestParamsCC params = GetParam();
  SetFile(params.filename);
  auto metric = CyclomaticComplexityMetric();
  MetricResult::ValueType result = metric.Calculate(*functions.begin()).value;
  EXPECT_EQ(std::get<int>(result), params.expected_result);
}

INSTANTIATE_TEST_SUITE_P(Add, CyclomaticComplexty,
                         testing::Values(TestParamsCC{"many_lines.py", 1},
                                         TestParamsCC{"comments.py", 0},
                                         TestParamsCC{"exceptions.py", 4},
                                         TestParamsCC{"if.py", 1},
                                         TestParamsCC{"loops.py", 3},
                                         TestParamsCC{"many_parameters.py", 1},
                                         TestParamsCC{"match_case.py", 4},
                                         TestParamsCC{"nested_if.py", 5},
                                         TestParamsCC{"simple.py", 1},
                                         TestParamsCC{"ternary.py", 0}));

} // namespace analyser::metric::metric_impl
