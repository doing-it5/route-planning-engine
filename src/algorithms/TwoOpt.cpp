#include "algorithms/TwoOpt.h"

#include <algorithm>
#include <stdexcept>
#include <limits>

// ─── 2-opt implementation ─────────────────────────────────────────────────────

TwoOptResult TwoOpt::improve(const std::vector<int64_t>& tour,
                              const std::vector<std::vector<double>>& costMatrix) {
    const int n = static_cast<int>(tour.size());
    if (n < 4) {
        // Nothing to improve for tours of < 4 cities
        TwoOptResult r;
        r.tour = tour;
        double total = 0.0;
        if (n > 1) {
            for (int i = 0; i < n; ++i) {
                if (i < static_cast<int>(costMatrix.size()) &&
                    ((i + 1) % n) < static_cast<int>(costMatrix[i].size())) {
                    total += costMatrix[i][(i + 1) % n];
                }
            }
        }
        r.totalDistance = total;
        return r;
    }

    // Work with index positions into the original tour vector
    std::vector<int> order(n);
    for (int i = 0; i < n; ++i) order[i] = i;

    bool improved = true;
    int  iters    = 0;

    while (improved) {
        improved = false;
        for (int i = 0; i < n - 1; ++i) {
            for (int j = i + 2; j < n; ++j) {
                // Skip wrap-around edge that connects j back to i=0
                if (i == 0 && j == n - 1) continue;

                const int a = order[i];
                const int b = order[i + 1];
                const int c = order[j];
                const int d = order[(j + 1) % n];

                const double before = costMatrix[a][b] + costMatrix[c][d];
                const double after  = costMatrix[a][c] + costMatrix[b][d];

                if (after < before - 1e-9) {
                    // Reverse the segment between i+1 and j
                    std::reverse(order.begin() + i + 1, order.begin() + j + 1);
                    improved = true;
                    ++iters;
                }
            }
        }
    }

    TwoOptResult result;
    result.iterations = iters;
    result.tour.reserve(n);
    for (int idx : order) {
        result.tour.push_back(tour[idx]);
    }

    // Compute final cost
    for (int i = 0; i < n; ++i) {
        result.totalDistance += costMatrix[order[i]][order[(i + 1) % n]];
    }

    return result;
}
