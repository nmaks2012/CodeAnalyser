#include "metric_accumulator_impl/sum_average_accumulator.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

namespace analyser::metric_accumulator::metric_accumulator_impl::test {

TEST(TestSumAverageAccumulator, AccumulateToFinalized) {
  SumAverageAccumulator accum;
  accum.Finalize();
  EXPECT_THROW(accum.Accumulate({"name", "UNKNOWN"}), std::runtime_error);
}

TEST(TestSumAverageAccumulator, GetNonFinalize) {
  SumAverageAccumulator accum;
  EXPECT_THROW(accum.Get(), std::runtime_error);
}

TEST(TestSumAverageAccumulator, AccumulateInt) {
  SumAverageAccumulator accum;
  accum.Accumulate({"name", 10});
  accum.Accumulate({"name", 10});
  accum.Accumulate({"name", 40});
  EXPECT_THROW(accum.Get(), std::runtime_error);
  accum.Finalize();
  EXPECT_DOUBLE_EQ(accum.Get().average, 20.0);

  accum.Reset();
  accum.Accumulate({"name", 10});
  EXPECT_THROW(accum.Get(), std::runtime_error);
  accum.Finalize();
  EXPECT_DOUBLE_EQ(accum.Get().average, 10.0);
  EXPECT_EQ(accum.Get().sum, 10);
}

TEST(TestSumAverageAccumulator, AccumulateString) {
  SumAverageAccumulator accum;
  accum.Accumulate({"name", "UNKNOWN"});
  accum.Accumulate({"name", "UNKNOWN"});
  accum.Finalize();
  EXPECT_DOUBLE_EQ(accum.Get().average, 0.0);
  EXPECT_EQ(accum.Get().sum, 0);
}

TEST(TestSumAverageAccumulator, Reset) {

  SumAverageAccumulator accum;
  accum.Accumulate({"name", 20});
  accum.Accumulate({"name", 30});
  accum.Finalize();
  accum.Reset();
  EXPECT_THROW(accum.Get(), std::runtime_error);
  accum.Finalize();
  EXPECT_DOUBLE_EQ(accum.Get().average, 0.0);
  EXPECT_EQ(accum.Get().sum, 0);
}

}  // namespace analyser::metric_accumulator::metric_accumulator_impl::test
