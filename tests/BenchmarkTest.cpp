#include <gtest/gtest.h>
#include "benchmark/Benchmark.h"
#include <thread>
#include <chrono>

TEST(BenchmarkTest, RunsFunctionAndMeasuresTime) {
    auto result = Benchmark::run("SleepBenchmark", []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }, 2);

    EXPECT_EQ(result.name, "SleepBenchmark");
    EXPECT_EQ(result.iterations, 2);
    EXPECT_GE(result.elapsedMs, 10.0);
}
