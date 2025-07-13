#include "metric_impl/naming_style.hpp"
#include "metric.hpp"

#include <gtest/gtest.h>
#include <print>
#include <string>

namespace analyser::metric::metric_impl {

using namespace std::literals;

const std::filesystem::path test_data_dir = TEST_DATA_DIR;

struct TestParamsNS {
  std::string filename;
  std::string expected_result;
};

class NamingStyle : public ::testing::TestWithParam<TestParamsNS> {
protected:
  void SetFile(const std::string &filename) {
    functions = extractor.Get({filename});
  }
  analyser::function::FunctionExtractor extractor;
  std::vector<function::Function> functions;
};

TEST_P(NamingStyle, Test) {
  TestParamsNS params = GetParam();
  SetFile((test_data_dir / params.filename).string());
  auto metric = NamingStyleMetric();
  MetricResult::ValueType result = metric.Calculate(*functions.begin()).value;
  EXPECT_EQ(std::get<std::string>(result), params.expected_result);
}

INSTANTIATE_TEST_SUITE_P(
    Add, NamingStyle,
    testing::Values(TestParamsNS{"many_lines.py", "LOWERCASE"s},
                    TestParamsNS{"comments.py", "UNKNOWN"s},
                    TestParamsNS{"exceptions.py", "UNKNOWN"s},
                    TestParamsNS{"if.py", "CAMELCASE"s},
                    TestParamsNS{"loops.py", "PASCALCASE"s},
                    TestParamsNS{"many_parameters.py", "UNKNOWN"s},
                    TestParamsNS{"match_case.py", "UNKNOWN"s},
                    TestParamsNS{"nested_if.py", "PASCALCASE"s},
                    TestParamsNS{"simple.py", "SNAKECASE"s},
                    TestParamsNS{"ternary.py", "UNKNOWN"s}));

} // namespace analyser::metric::metric_impl
