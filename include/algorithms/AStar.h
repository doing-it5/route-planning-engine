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
        : distance(std::numeric_limits<double>::max()),
          nodeExpansions(0),
          found(false) {}
};

// ─── A* class ────────────────────────────────────────────────────────────────

class AStar {
public:
    /// Find the shortest path from @p start to @p end using the Haversine
    /// great-circle distance as an admissible heuristic.
    static AStarResult findShortest(Graph& graph, int64_t start, int64_t end);
};

#endif // ASTAR_H
