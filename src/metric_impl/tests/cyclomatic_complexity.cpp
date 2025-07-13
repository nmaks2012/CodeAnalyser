#include "metric_impl/cyclomatic_complexity.hpp"
#include "metric.hpp"

#include <gtest/gtest.h>

namespace analyser::metric::metric_impl {

const std::filesystem::path test_data_dir = TEST_DATA_DIR;

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
  SetFile((test_data_dir / params.filename).string());
  auto metric = CyclomaticComplexityMetric();
  MetricResult::ValueType result = metric.Calculate(*functions.begin()).value;
  EXPECT_EQ(std::get<int>(result), params.expected_result);
}

INSTANTIATE_TEST_SUITE_P(Add, CyclomaticComplexty,
                         testing::Values(TestParamsCC{"many_lines.py", 2},
                                         TestParamsCC{"comments.py", 1},
                                         TestParamsCC{"exceptions.py", 5},
                                         TestParamsCC{"if.py", 2},
                                         TestParamsCC{"loops.py", 4},
                                         TestParamsCC{"many_parameters.py", 2},
                                         TestParamsCC{"match_case.py", 5},
                                         TestParamsCC{"nested_if.py", 6},
                                         TestParamsCC{"simple.py", 2},
                                         TestParamsCC{"ternary.py", 1}));

} // namespace analyser::metric::metric_impl
