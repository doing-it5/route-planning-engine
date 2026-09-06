#include "benchmark/Benchmark.h"

#include <chrono>
#include <iostream>
#include <iomanip>

#ifdef __linux__
#  include <sys/resource.h>
#endif

// ─── peakMemoryKB ─────────────────────────────────────────────────────────────

long Benchmark::peakMemoryKB() {
#ifdef __linux__
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;  // already in KB on Linux
#else
    return 0;
#endif
}

// ─── run ─────────────────────────────────────────────────────────────────────

BenchmarkResult Benchmark::run(const std::string& name,
                               std::function<void()> fn,
                               int iterations) {
    const auto t0 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        fn();
    }

    const auto t1 = std::chrono::high_resolution_clock::now();
    const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    return BenchmarkResult(name, ms, peakMemoryKB(), iterations);
}

// ─── printResults ─────────────────────────────────────────────────────────────

void Benchmark::printResults(const std::vector<BenchmarkResult>& results) {
    std::cout << "\n";
    std::cout << std::left
              << std::setw(30) << "Benchmark"
              << std::setw(15) << "Total (ms)"
              << std::setw(15) << "Per-iter (ms)"
              << std::setw(12) << "Iterations"
              << std::setw(14) << "Peak Mem (KB)"
              << "\n";
    std::cout << std::string(86, '-') << "\n";

    for (const auto& r : results) {
        double perIter = r.iterations > 0 ? r.elapsedMs / r.iterations : 0.0;
        std::cout << std::left
                  << std::setw(30) << r.name
                  << std::setw(15) << std::fixed << std::setprecision(3) << r.elapsedMs
                  << std::setw(15) << std::fixed << std::setprecision(3) << perIter
                  << std::setw(12) << r.iterations
                  << std::setw(14) << r.peakMemoryKB
                  << "\n";
    }
    std::cout << "\n";
}
