#ifndef ASTAR_H
#define ASTAR_H

#include "graph/Graph.h"

#include <limits>
#include <vector>
#include <unordered_map>
#include <queue>
#include <cstdint>

// ─── Result ───────────────────────────────────────────────────────────────────

struct AStarResult {
    double               distance;       // total cost along the path
    std::vector<int64_t> path;           // ordered node IDs from start to end
    int                  nodeExpansions; // nodes popped from the open set
    bool                 found;          // false when no path exists

    AStarResult()
        : distance(0.0),
          nodeExpansions(0),
          found(false) {}
    // NOTE: distance is only meaningful when found == true.
};

// ─── A* class ────────────────────────────────────────────────────────────────

class AStar {
public:
    /// Find the shortest path from @p start to @p end using the Haversine
    /// great-circle distance as an admissible heuristic.
    [[nodiscard]] static AStarResult findShortest(const Graph& graph, int64_t start, int64_t end);
};

#endif // ASTAR_H
