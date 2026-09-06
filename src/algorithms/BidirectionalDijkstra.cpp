#include "algorithms/BidirectionalDijkstra.h"

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

BiDijkstraResult BidirectionalDijkstra::findShortest(const Graph& graph,
                                                      int64_t start,
                                                      int64_t end) {
    BiDijkstraResult result;

    if (!graph.nodeExists(start) || !graph.nodeExists(end)) {
        return result;
    }

    if (start == end) {
        result.distance = 0.0;
        result.path     = {start};
        result.found    = true;
        return result;
    }

    const double INF = (std::numeric_limits<double>::max)();

    // ── Forward and backward distance maps ───────────────────────────────────
    std::unordered_map<int64_t, double>  distF, distB;
    std::unordered_map<int64_t, int64_t> prevF, prevB;

    for (const auto& [id, _] : graph.getAllNodes()) {
        distF[id] = INF;
        distB[id] = INF;
    }
    distF[start] = 0.0;
    distB[end]   = 0.0;

    std::priority_queue<PQNode, std::vector<PQNode>, std::greater<PQNode>> pqF, pqB;
    pqF.push({start, 0.0});
    pqB.push({end,   0.0});

    std::unordered_map<int64_t, bool> settledF, settledB;

    double  best    = INF;
    int64_t midNode = -1;

    // ── Main loop: alternate between forward and backward ────────────────────
    while (!pqF.empty() || !pqB.empty()) {

        // ── Forward step ─────────────────────────────────────────────────────
        if (!pqF.empty()) {
            auto [u, d] = pqF.top(); pqF.pop();
            ++result.nodeExpansions;

            if (d > distF[u]) goto backward_step;  // stale
            settledF[u] = true;

            // Termination check
            if (settledB.count(u) && distF[u] + distB[u] < best) {
                best    = distF[u] + distB[u];
                midNode = u;
            }
            if (distF[u] + distB[u] >= best) goto backward_step;

            for (const Edge& edge : graph.getNeighbors(u)) {
                const double newDist = distF[u] + edge.weight;
                if (newDist < distF[edge.to]) {
                    distF[edge.to] = newDist;
                    prevF[edge.to] = u;
                    pqF.push({edge.to, newDist});
                    if (settledB.count(edge.to) && newDist + distB[edge.to] < best) {
                        best    = newDist + distB[edge.to];
                        midNode = edge.to;
                    }
                }
            }
        }

        backward_step:
        // ── Backward step ────────────────────────────────────────────────────
        if (!pqB.empty()) {
            auto [u, d] = pqB.top(); pqB.pop();
            ++result.nodeExpansions;

            if (d > distB[u]) continue;  // stale
            settledB[u] = true;

            if (settledF.count(u) && distF[u] + distB[u] < best) {
                best    = distF[u] + distB[u];
                midNode = u;
            }
            if (distB[u] + distF[u] >= best) continue;

            for (const Edge& edge : graph.getNeighbors(u)) {
                // Traverse reverse edges: treat edge.to as coming TO u
                // (works correctly when the graph has symmetric bidirectional edges)
                const double newDist = distB[u] + edge.weight;
                if (newDist < distB[edge.to]) {
                    distB[edge.to] = newDist;
                    prevB[edge.to] = u;
                    pqB.push({edge.to, newDist});
                    if (settledF.count(edge.to) && distF[edge.to] + newDist < best) {
                        best    = distF[edge.to] + newDist;
                        midNode = edge.to;
                    }
                }
            }
        }
    }

    if (midNode == -1 || best >= INF) return result;  // no path

    // ── Reconstruct path: forward half + backward half ────────────────────────
    result.distance = best;
    result.found    = true;

    // Forward: start → midNode
    std::vector<int64_t> fwd;
    for (int64_t cur = midNode; cur != start; ) {
        fwd.push_back(cur);
        auto it = prevF.find(cur);
        if (it == prevF.end()) { result.found = false; return result; }
        cur = it->second;
    }
    fwd.push_back(start);
    std::reverse(fwd.begin(), fwd.end());

    // Backward: midNode → end
    std::vector<int64_t> bwd;
    for (int64_t cur = midNode; cur != end; ) {
        auto it = prevB.find(cur);
        if (it == prevB.end()) break;
        cur = it->second;
        bwd.push_back(cur);
    }

    result.path = fwd;
    result.path.insert(result.path.end(), bwd.begin(), bwd.end());

    return result;
}
