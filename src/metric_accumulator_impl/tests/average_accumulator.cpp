#include "metric_accumulator_impl/average_accumulator.hpp"
#include "metric.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

namespace analyser::metric_accumulator::metric_accumulator_impl::test {

TEST(TestAverageAccumulator, InvalidParam) {
  AverageAccumulator accum;
  accum.Accumulate({"name", "UNKNOWN"});
  accum.Finalize();
  EXPECT_DOUBLE_EQ(accum.Get(), 0.0);
}

TEST(TestAverageAccumulator, FinalizeEmpty) {
  AverageAccumulator accum;
  accum.Finalize();
  EXPECT_DOUBLE_EQ(accum.Get(), 0.0);
}

TEST(TestAverageAccumulator, Finalize) {
  metric::MetricResult result1{"name", 10};
  metric::MetricResult result2{"name", 20};
  AverageAccumulator accum;
  accum.Accumulate(result1);
  accum.Accumulate(result2);
  EXPECT_THROW(accum.Get(), std::runtime_error);
  accum.Finalize();
  EXPECT_DOUBLE_EQ(accum.Get(), 15);
}

TEST(TestAverageAccumulator, Reset) {
  metric::MetricResult result1{"name", 10};
  metric::MetricResult result2{"name", 20};
  AverageAccumulator accum;
  accum.Accumulate(result1);
  accum.Accumulate(result2);
  EXPECT_THROW(accum.Get(), std::runtime_error);
  accum.Finalize();
  EXPECT_DOUBLE_EQ(accum.Get(), 15);
  accum.Reset();
  EXPECT_THROW(accum.Get(), std::runtime_error);
}

TEST(TestAverageAccumulator, GetNotFinalize) {
  AverageAccumulator accum;
  EXPECT_THROW(accum.Get(), std::runtime_error);
}

} // namespace analyser::metric_accumulator::metric_accumulator_impl::test
