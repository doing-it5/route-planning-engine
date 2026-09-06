#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph/Graph.h"

#include <limits>
#include <vector>
#include <unordered_map>
#include <queue>
#include <cstdint>

// ─── Result ───────────────────────────────────────────────────────────────────

struct DijkstraResult {
    double              distance;       // total cost along the path
    std::vector<int64_t> path;          // ordered node IDs from start to end
    int                 nodeExpansions; // number of nodes popped from the queue
    bool                found;          // false when no path exists

    DijkstraResult()
        : distance(0.0),
          nodeExpansions(0),
          found(false) {}
    // NOTE: distance is only meaningful when found == true.
};

// ─── Dijkstra class ───────────────────────────────────────────────────────────

class Dijkstra {
public:
    /// Find the shortest path from @p start to @p end in @p graph.
    [[nodiscard]] static DijkstraResult findShortest(const Graph& graph, int64_t start, int64_t end);
};

#endif // DIJKSTRA_H
