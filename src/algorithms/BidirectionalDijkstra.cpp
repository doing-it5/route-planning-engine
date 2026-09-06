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
using MinPQ = std::priority_queue<PQNode, std::vector<PQNode>, std::greater<PQNode>>;
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

    // ── Forward and backward distance/parent maps ─────────────────────────────
    std::unordered_map<int64_t, double>  distF, distB;
    std::unordered_map<int64_t, int64_t> prevF, prevB;
    std::unordered_map<int64_t, bool>    settledF, settledB;

    for (const auto& [id, node] : graph.getAllNodes()) {
        distF[id] = INF;
        distB[id] = INF;
    }
    distF[start] = 0.0;
    distB[end]   = 0.0;

    MinPQ pqF, pqB;
    pqF.push({start, 0.0});
    pqB.push({end,   0.0});

    double  best    = INF;
    int64_t midNode = -1;

    // ── Alternate forward / backward steps until queues exhausted ─────────────
    while (!pqF.empty() || !pqB.empty()) {

        // Forward step
        if (!pqF.empty()) {
            auto [u, d] = pqF.top(); pqF.pop();
            ++result.nodeExpansions;

            if (d > distF[u]) { /* stale */ }
            else {
                settledF[u] = true;

                if (settledB.count(u) && distF[u] + distB[u] < best) {
                    best    = distF[u] + distB[u];
                    midNode = u;
                }

                if (distF[u] < best) {
                    for (const Edge& edge : graph.getNeighbors(u)) {
                        const double newDist = distF[u] + edge.weight;
                        if (newDist < distF[edge.to]) {
                            distF[edge.to] = newDist;
                            prevF[edge.to] = u;
                            pqF.push({edge.to, newDist});
                            if (settledB.count(edge.to) &&
                                newDist + distB[edge.to] < best) {
                                best    = newDist + distB[edge.to];
                                midNode = edge.to;
                            }
                        }
                    }
                }
            }
        }

        // Backward step
        if (!pqB.empty()) {
            auto [u, d] = pqB.top(); pqB.pop();
            ++result.nodeExpansions;

            if (d > distB[u]) { /* stale */ }
            else {
                settledB[u] = true;

                if (settledF.count(u) && distF[u] + distB[u] < best) {
                    best    = distF[u] + distB[u];
                    midNode = u;
                }

                if (distB[u] < best) {
                    for (const Edge& edge : graph.getNeighbors(u)) {
                        const double newDist = distB[u] + edge.weight;
                        if (newDist < distB[edge.to]) {
                            distB[edge.to] = newDist;
                            prevB[edge.to] = u;
                            pqB.push({edge.to, newDist});
                            if (settledF.count(edge.to) &&
                                distF[edge.to] + newDist < best) {
                                best    = distF[edge.to] + newDist;
                                midNode = edge.to;
                            }
                        }
                    }
                }
            }
        }
    }

    if (midNode == -1 || best >= INF) return result;  // no path

    // ── Reconstruct: forward half (start → midNode) ───────────────────────────
    std::vector<int64_t> fwd;
    for (int64_t cur = midNode; cur != start; ) {
        fwd.push_back(cur);
        auto it = prevF.find(cur);
        if (it == prevF.end()) { return result; }  // reconstruction failure
        cur = it->second;
    }
    fwd.push_back(start);
    std::reverse(fwd.begin(), fwd.end());

    // ── Reconstruct: backward half (midNode → end) ────────────────────────────
    std::vector<int64_t> bwd;
    for (int64_t cur = midNode; cur != end; ) {
        auto it = prevB.find(cur);
        if (it == prevB.end()) break;
        cur = it->second;
        bwd.push_back(cur);
    }

    result.path = fwd;
    result.path.insert(result.path.end(), bwd.begin(), bwd.end());
    result.distance = best;
    result.found    = true;

    return result;
}
