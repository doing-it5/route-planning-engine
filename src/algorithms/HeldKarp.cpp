#include "algorithms/HeldKarp.h"

#include <algorithm>
#include <stdexcept>
#include <cstdint>

// ─── Distance matrix builder ──────────────────────────────────────────────────

std::vector<std::vector<double>>
HeldKarp::buildDistanceMatrix(const Graph& graph, const std::vector<int64_t>& stops) {
    const int n = static_cast<int>(stops.size());
    const double INF = (std::numeric_limits<double>::max)();
    std::vector<std::vector<double>> dist(n, std::vector<double>(n, INF));

    for (int i = 0; i < n; ++i) {
        dist[i][i] = 0.0;
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            DijkstraResult res = Dijkstra::findShortest(graph, stops[i], stops[j]);
            if (res.found) {
                dist[i][j] = res.distance;
            }
        }
    }
    return dist;
}

// ─── Held-Karp DP ─────────────────────────────────────────────────────────────

HeldKarpResult HeldKarp::solve(const Graph& graph, const std::vector<int64_t>& stops) {
    HeldKarpResult result;

    const int n = static_cast<int>(stops.size());
    if (n < 2) {
        throw std::invalid_argument("HeldKarp::solve requires at least 2 stops");
    }
    if (n > 20) {
        throw std::invalid_argument("HeldKarp::solve supports at most 20 stops (2^20 states)");
    }

    // ── Build pairwise distance matrix ────────────────────────────────────
    const auto dist = buildDistanceMatrix(graph, stops);
    const double INF = (std::numeric_limits<double>::max)() / 2.0;

    // ── DP table: dp[mask][i] = min cost to visit all nodes in mask,
    //    ending at node i, starting from node 0. ─────────────────────────
    const int states = 1 << n;
    std::vector<std::vector<double>>  dp  (states, std::vector<double>(n, INF));
    std::vector<std::vector<int>>     par (states, std::vector<int>(n, -1));

    dp[1][0] = 0.0;  // start at node 0, only node 0 visited

    for (int mask = 1; mask < states; ++mask) {
        for (int u = 0; u < n; ++u) {
            if (!(mask & (1 << u))) continue;     // u not in mask
            if (dp[mask][u] >= INF) continue;     // unreachable

            for (int v = 0; v < n; ++v) {
                if (mask & (1 << v)) continue;    // v already visited
                if (dist[u][v] >= INF) continue;  // no path u→v

                const int   newMask = mask | (1 << v);
                const double newCost = dp[mask][u] + dist[u][v];
                if (newCost < dp[newMask][v]) {
                    dp[newMask][v]  = newCost;
                    par[newMask][v] = u;
                }
            }
        }
    }

    // ── Find best final state (all visited, return to node 0) ────────────
    const int fullMask = states - 1;
    double    bestCost = INF;
    int       lastNode = -1;

    for (int u = 1; u < n; ++u) {
        if (dp[fullMask][u] >= INF) continue;
        if (dist[u][0] >= INF) continue;
        const double cost = dp[fullMask][u] + dist[u][0];
        if (cost < bestCost) {
            bestCost = cost;
            lastNode = u;
        }
    }

    if (lastNode == -1) {
        return result;  // no valid tour
    }

    // ── Reconstruct tour ──────────────────────────────────────────────────
    result.totalDistance = bestCost;
    result.found         = true;

    std::vector<int> orderIdx;
    int mask = fullMask;
    int cur  = lastNode;
    while (cur != -1) {
        orderIdx.push_back(cur);
        int prev = par[mask][cur];
        mask ^= (1 << cur);
        cur   = prev;
    }
    std::reverse(orderIdx.begin(), orderIdx.end());

    for (int idx : orderIdx) {
        result.tour.push_back(stops[idx]);
    }

    return result;
}
