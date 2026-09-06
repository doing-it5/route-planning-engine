#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <string>
#include <cstdint>

// ─── Data Structures ────────────────────────────────────────────────────────

struct Node {
    int64_t id;
    double  latitude;
    double  longitude;

    Node() : id(-1), latitude(0.0), longitude(0.0) {}
    Node(int64_t id, double lat, double lon)
        : id(id), latitude(lat), longitude(lon) {}
};

struct Edge {
    int64_t from;
    int64_t to;
    double  weight;    // routing cost (distance or travel-time)
    double  distance;  // actual distance in metres

    Edge() : from(-1), to(-1), weight(0.0), distance(0.0) {}
    Edge(int64_t f, int64_t t, double w, double d)
        : from(f), to(t), weight(w), distance(d) {}
};

// ─── Graph class ─────────────────────────────────────────────────────────────

class Graph {
public:
    // ── Mutations ────────────────────────────────────────────────────────────
    void addNode(int64_t id, double latitude, double longitude);
    void addEdge(int64_t from, int64_t to, double weight, double distance);

    // ── Queries ──────────────────────────────────────────────────────────────
    Node*               getNode(int64_t id);
    const Node*         getNode(int64_t id) const;
    bool                nodeExists(int64_t id) const;
    std::vector<Edge>&  getNeighbors(int64_t nodeId);
    const std::vector<Edge>& getNeighbors(int64_t nodeId) const;

    const std::unordered_map<int64_t, Node>&              getAllNodes()        const { return nodes; }
    const std::unordered_map<int64_t, std::vector<Edge>>& getAdjacencyList()  const { return adjacencyList; }

    size_t nodeCount() const { return nodes.size(); }
    size_t edgeCount() const;

    void clear();

    /// Remove a node and all its incident edges. No-op if node does not exist.
    void removeNode(int64_t id);

    /// Remove all edges from @p from to @p to. No-op if no such edge exists.
    void removeEdge(int64_t from, int64_t to);

    // ── Geometry helper ──────────────────────────────────────────────────────
    /// Returns great-circle distance in metres between two lat/lon points.
    static double haversineDistance(double lat1, double lon1,
                                    double lat2, double lon2);

private:
    std::unordered_map<int64_t, Node>              nodes;
    std::unordered_map<int64_t, std::vector<Edge>> adjacencyList;
};

#endif // GRAPH_H
