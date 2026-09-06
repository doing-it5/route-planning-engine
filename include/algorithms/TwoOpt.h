#ifndef TWOOPT_H
#define TWOOPT_H

#include <vector>
#include <cstdint>

// ─── Result ───────────────────────────────────────────────────────────────────

struct TwoOptResult {
    std::vector<int64_t> tour;          // improved ordered node IDs
    double               totalDistance; // total tour cost after improvement
    int                  iterations;    // number of improving swaps applied

    TwoOptResult() : totalDistance(0.0), iterations(0) {}
};

// ─── 2-opt local search ───────────────────────────────────────────────────────
///
/// Iteratively reverses sub-sequences of the tour to reduce total cost.
/// Converges when no improving swap exists (local optimum).
/// Time per iteration: O(n²)
///
class TwoOpt {
public:
    /// Improve @p tour using a symmetric cost matrix @p costMatrix.
    /// @p costMatrix[i][j] is the cost from tour[i] to tour[j].
    [[nodiscard]] static TwoOptResult improve(const std::vector<int64_t>& tour,
                                const std::vector<std::vector<double>>& costMatrix);
};

#endif // TWOOPT_H
