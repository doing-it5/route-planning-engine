#include "algorithms/AStar.h"

#include <queue>
#include <unordered_map>
#include <limits>
#include <cmath>
#include <algorithm>
#include <stdexcept>

// ─── Internal open-set node ───────────────────────────────────────────────────

namespace {
struct OpenNode {
    int64_t id;
    double  f;  // f = g + h
    bool operator>(const OpenNode& o) const { return f > o.f; }
};
} // anonymous namespace

// ─── Implementation ───────────────────────────────────────────────────────────

AStarResult AStar::findShortest(Graph& graph, int64_t start, int64_t end) {
    AStarResult result;

    if (!graph.nodeExists(start) || !graph.nodeExists(end)) {
        return result;
    }

    if (start == end) {
        result.distance = 0.0;
        result.path     = {start};
        result.found    = true;
        return result;
    }

    // Goal node coordinates for the heuristic
    const Node* goalNode = graph.getNode(end);

    // ── Heuristic: Haversine straight-line distance ───────────────────────
    auto heuristic = [&](int64_t id) -> double {
        const Node* n = graph.getNode(id);
        if (!n) return 0.0;
        return Graph::haversineDistance(n->latitude, n->longitude,
                                        goalNode->latitude, goalNode->longitude);
    };

    // ── Initialise ────────────────────────────────────────────────────────
    const double INF = (std::numeric_limits<double>::max)();
    std::unordered_map<int64_t, double>  g;   // known cost from start
    std::unordered_map<int64_t, int64_t> prev;

    for (const auto& [id, _] : graph.getAllNodes()) {
        g[id] = INF;
    }
    g[start] = 0.0;

    std::priority_queue<OpenNode, std::vector<OpenNode>, std::greater<OpenNode>> open;
    open.push({start, heuristic(start)});

    // ── Main loop ─────────────────────────────────────────────────────────
    while (!open.empty()) {
        auto [u, f_u] = open.top();
        open.pop();
        ++result.nodeExpansions;

        // Stale entry — skip
        if (f_u > g[u] + heuristic(u) + 1e-9) continue;

        if (u == end) break;

        for (const Edge& edge : graph.getNeighbors(u)) {
            const double tentative_g = g[u] + edge.weight;
            if (tentative_g < g[edge.to]) {
                g[edge.to]    = tentative_g;
                prev[edge.to] = u;
                open.push({edge.to, tentative_g + heuristic(edge.to)});
            }
        }
    }

    // ── Reconstruct path ──────────────────────────────────────────────────
    if (g[end] == INF) {
        return result;  // no path
    }

    result.distance = g[end];
    result.found    = true;

    for (int64_t cur = end; cur != start; cur = prev.at(cur)) {
        result.path.push_back(cur);
    }
    result.path.push_back(start);
    std::reverse(result.path.begin(), result.path.end());

    return result;
}
