#include "algorithms/Dijkstra.h"

#include <queue>
#include <unordered_map>
#include <limits>
#include <algorithm>

// ─── Internal priority-queue node ────────────────────────────────────────────

namespace {
struct PQNode {
    int64_t id;
    double  dist;
    bool operator>(const PQNode& o) const { return dist > o.dist; }
};
} // anonymous namespace

// ─── Implementation ───────────────────────────────────────────────────────────

DijkstraResult Dijkstra::findShortest(Graph& graph, int64_t start, int64_t end) {
    DijkstraResult result;

    if (!graph.nodeExists(start) || !graph.nodeExists(end)) {
        return result;  // found == false, distance == INF
    }

    if (start == end) {
        result.distance = 0.0;
        result.path     = {start};
        result.found    = true;
        return result;
    }

    // ── Initialise distance table ─────────────────────────────────────────
    const double INF = std::numeric_limits<double>::max();
    std::unordered_map<int64_t, double>  dist;
    std::unordered_map<int64_t, int64_t> prev;

    for (const auto& [id, _] : graph.getAllNodes()) {
        dist[id] = INF;
    }
    dist[start] = 0.0;

    std::priority_queue<PQNode, std::vector<PQNode>, std::greater<PQNode>> pq;
    pq.push({start, 0.0});

    // ── Main loop ─────────────────────────────────────────────────────────
    while (!pq.empty()) {
        auto [u, d] = pq.top();
        pq.pop();
        ++result.nodeExpansions;

        // Stale entry in the priority queue — skip
        if (d > dist[u]) continue;

        // Early exit once target is settled
        if (u == end) break;

        for (const Edge& edge : graph.getNeighbors(u)) {
            const double newDist = dist[u] + edge.weight;
            if (newDist < dist[edge.to]) {
                dist[edge.to] = newDist;
                prev[edge.to] = u;
                pq.push({edge.to, newDist});
            }
        }
    }

    // ── Reconstruct path ──────────────────────────────────────────────────
    if (dist[end] == INF) {
        return result;  // no path found
    }

    result.distance = dist[end];
    result.found    = true;

    for (int64_t cur = end; cur != start; cur = prev.at(cur)) {
        result.path.push_back(cur);
    }
    result.path.push_back(start);
    std::reverse(result.path.begin(), result.path.end());

    return result;
}
