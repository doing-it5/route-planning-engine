#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>
#include <string>
#include <functional>
#include <vector>
#include <iostream>
#include <iomanip>

// ─── Benchmark result ────────────────────────────────────────────────────────

struct BenchmarkResult {
    std::string name;
    double      elapsedMs;     // wall-clock time in milliseconds
    long        peakMemoryKB;  // resident set size at end of run (Linux only; 0 on Windows)
    int         iterations;

    BenchmarkResult(std::string n, double ms, long mem, int iters)
        : name(std::move(n)), elapsedMs(ms), peakMemoryKB(mem), iterations(iters) {}
};

// ─── Benchmark class ──────────────────────────────────────────────────────────

class Benchmark {
public:
    /// Run @p fn @p iterations times and return aggregate results.
    static BenchmarkResult run(const std::string& name,
                               std::function<void()> fn,
                               int iterations = 1);

    /// Pretty-print a list of results as a table.
    static void printResults(const std::vector<BenchmarkResult>& results);

    /// Returns peak RSS in KB on Linux, 0 on other platforms.
    static long peakMemoryKB();
};

#endif // BENCHMARK_H
