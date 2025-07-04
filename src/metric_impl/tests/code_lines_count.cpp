#include "metric_impl/code_lines_count.hpp"
#include "function.hpp"
#include "metric.hpp"

#include <algorithm>
#include <gtest/gtest.h>
#include <ranges>
#include <vector>

namespace analyser::metric::metric_impl {

struct TestParamsCL {
  std::string filename;
  int expected_result;
};

class CodeLinesCount : public ::testing::TestWithParam<TestParamsCL> {
protected:
  void SetFile(const std::string &filename) {
    functions = extractor.Get({filename});
  }
  analyser::function::FunctionExtractor extractor;
  std::vector<function::Function> functions;
};

TEST_P(CodeLinesCount, Test) {
  TestParamsCL params = GetParam();
  SetFile(params.filename);
  auto metric = CodeLinesCountMetric();
  MetricResult::ValueType result = metric.Calculate(*functions.begin()).value;
  EXPECT_EQ(std::get<int>(result), params.expected_result);
}

INSTANTIATE_TEST_SUITE_P(Add, CodeLinesCount,
                         testing::Values(TestParamsCL{"many_lines.py", 10},
                                         TestParamsCL{"comments.py", 3},
                                         TestParamsCL{"exceptions.py", 7},
                                         TestParamsCL{"if.py", 3},
                                         TestParamsCL{"loops.py", 6},
                                         TestParamsCL{"many_parameters.py", 1},
                                         TestParamsCL{"match_case.py", 7},
                                         TestParamsCL{"nested_if.py", 8},
                                         TestParamsCL{"simple.py", 5},
                                         TestParamsCL{"ternary.py", 1}));

TEST(CodeLinesCount, ManyFunctions) {

  analyser::function::FunctionExtractor extractor;
  std::vector<function::Function> functions = extractor.Get({"advanced_processor.py"});
  auto metric = CodeLinesCountMetric();
  MetricResults result;

  rs::for_each(functions, [&](function::Function &func) {
    result.push_back(metric.Calculate(func));
  });

  std::vector<int> expected_result{2, 2, 1, 5};
  auto zipped = rv::zip(result, expected_result);

  EXPECT_TRUE( expected_result.size() == result.size());
  EXPECT_TRUE(
    rs::all_of(zipped, [](const auto& item){
      const auto [res, expect] = item;
      return std::get<int>(res.value) == expect;
    })
  );
}

} // namespace analyser::metric::metric_impl
