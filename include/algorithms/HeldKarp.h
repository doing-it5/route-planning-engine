#ifndef HELDKARP_H
#define HELDKARP_H

#include "graph/Graph.h"
#include "algorithms/Dijkstra.h"

#include <vector>
#include <limits>
#include <cstdint>

// ─── Result ───────────────────────────────────────────────────────────────────

struct HeldKarpResult {
    double               totalDistance; // sum of edge weights along the optimal tour
    std::vector<int64_t> tour;          // ordered node IDs (does NOT repeat start)
    bool                 found;

    HeldKarpResult()
        : totalDistance(0.0), found(false) {}
    // NOTE: totalDistance is only meaningful when found == true.
};

// ─── Held-Karp TSP solver ─────────────────────────────────────────────────────
///
/// Exact bitmask DP solver — optimal for up to ~16 stops.
/// Time:  O(n² · 2ⁿ)   Space: O(n · 2ⁿ)
///
class HeldKarp {
public:
    /// Solve the TSP for @p stops in @p graph.
    /// @p stops must contain at least 2 nodes that all exist in the graph.
    /// The tour begins and ends at stops[0].
    [[nodiscard]] static HeldKarpResult solve(const Graph& graph, const std::vector<int64_t>& stops);

private:
    /// Build a distance matrix by running Dijkstra between every pair of stops.
    static std::vector<std::vector<double>>
    buildDistanceMatrix(const Graph& graph, const std::vector<int64_t>& stops);
};

#endif // HELDKARP_H
