#ifndef BIDIRECTIONAL_DIJKSTRA_H
#define BIDIRECTIONAL_DIJKSTRA_H

#include "graph/Graph.h"

#include <vector>
#include <cstdint>
#include <limits>

// ─── Result ───────────────────────────────────────────────────────────────────

struct BiDijkstraResult {
    double               distance;       // total cost along the path
    std::vector<int64_t> path;           // ordered node IDs from start to end
    int                  nodeExpansions; // total nodes settled in both directions
    bool                 found;

    BiDijkstraResult() : distance(0.0), nodeExpansions(0), found(false) {}
    // NOTE: distance is only meaningful when found == true.
};

// ─── Bidirectional Dijkstra ───────────────────────────────────────────────────
///
/// Runs two simultaneous Dijkstra searches — one forward from start,
/// one backward from end — meeting in the middle.
/// ~2× faster than standard Dijkstra on large graphs with uniform edge costs.
///
class BidirectionalDijkstra {
public:
    /// Find the shortest path from @p start to @p end in @p graph.
    /// @p graph must support bidirectional adjacency (i.e., reverse edges must exist).
    [[nodiscard]] static BiDijkstraResult findShortest(const Graph& graph,
                                                       int64_t start,
                                                       int64_t end);
};

#endif // BIDIRECTIONAL_DIJKSTRA_H
