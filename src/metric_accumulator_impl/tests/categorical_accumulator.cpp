#include "metric_accumulator_impl/categorical_accumulator.hpp"
#include "metric.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

namespace analyser::metric_accumulator::metric_accumulator_impl {

TEST(TestCategoricalAccumulator, AccumulateToFinalized) {
  CategoricalAccumulator accum;
  accum.Finalize();
  EXPECT_THROW(accum.Accumulate({"name", "UNKNOWN"}), std::runtime_error);
}

TEST(TestCategoricalAccumulator, GetNonFinalize) {
  CategoricalAccumulator accum;
  EXPECT_THROW(accum.Get(), std::runtime_error);
}

TEST(TestCategoricalAccumulator, AccumulateString) {
  CategoricalAccumulator accum;
  accum.Accumulate({"name", "UNKNOWN"});
  accum.Accumulate({"name", "UNKNOWN"});
  accum.Accumulate({"name", "CAMELCASE"});
  EXPECT_THROW(accum.Get(), std::runtime_error);
  accum.Finalize();
  EXPECT_EQ(accum.Get().size(), 2);
  EXPECT_TRUE(accum.Get().contains("UNKNOWN") && accum.Get().at("UNKNOWN") == 2);
  EXPECT_TRUE(accum.Get().contains("CAMELCASE") && accum.Get().at("CAMELCASE") == 1);

  accum.Reset();
  accum.Accumulate({"name", "CAMELCASE"});
  accum.Accumulate({"name", "CAMELCASE"});
  EXPECT_THROW(accum.Get(), std::runtime_error);
  accum.Finalize();
  EXPECT_EQ(accum.Get().size(), 1);
  EXPECT_TRUE(accum.Get().contains("CAMELCASE") && accum.Get().at("CAMELCASE") == 2);
}

TEST(TestCategoricalAccumulator, AccumulateInt) {
  CategoricalAccumulator accum;
  accum.Accumulate({"name", 1});
  accum.Accumulate({"name", 1});
  accum.Finalize();
  EXPECT_EQ(accum.Get().size(), 0);
}

TEST(TestCategoricalAccumulator, Reset) {

  CategoricalAccumulator accum;
  accum.Accumulate({"name", "UNKNOWN"});
  accum.Accumulate({"name", "CAMELCASE"});
  accum.Finalize();
  accum.Reset();
  EXPECT_THROW(accum.Get(), std::runtime_error);
  accum.Finalize();
  EXPECT_EQ(accum.Get().size(), 0);
}

} // namespace analyser::metric_accumulator::metric_accumulator_impl
