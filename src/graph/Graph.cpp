#include "graph/Graph.h"

#include <cmath>
#include <stdexcept>

static constexpr double PI = 3.14159265358979323846;
static constexpr double EARTH_RADIUS_M = 6371000.0;  // metres

// ─── Node operations ─────────────────────────────────────────────────────────

void Graph::addNode(int64_t id, double latitude, double longitude) {
    if (!nodeExists(id)) {
        nodes[id]         = Node(id, latitude, longitude);
        adjacencyList[id] = {};  // ensure entry exists even with no edges
    }
}

Node* Graph::getNode(int64_t id) {
    auto it = nodes.find(id);
    return (it != nodes.end()) ? &it->second : nullptr;
}

const Node* Graph::getNode(int64_t id) const {
    auto it = nodes.find(id);
    return (it != nodes.end()) ? &it->second : nullptr;
}

bool Graph::nodeExists(int64_t id) const {
    return nodes.find(id) != nodes.end();
}

// ─── Edge operations ─────────────────────────────────────────────────────────

void Graph::addEdge(int64_t from, int64_t to, double weight, double distance) {
    if (!nodeExists(from)) {
        throw std::invalid_argument("addEdge: source node " + std::to_string(from) + " does not exist");
    }
    if (!nodeExists(to)) {
        throw std::invalid_argument("addEdge: destination node " + std::to_string(to) + " does not exist");
    }
    adjacencyList[from].emplace_back(from, to, weight, distance);
}

std::vector<Edge>& Graph::getNeighbors(int64_t nodeId) {
    auto it = adjacencyList.find(nodeId);
    if (it == adjacencyList.end()) {
        throw std::invalid_argument("getNeighbors: node " + std::to_string(nodeId) + " does not exist");
    }
    return it->second;
}

const std::vector<Edge>& Graph::getNeighbors(int64_t nodeId) const {
    auto it = adjacencyList.find(nodeId);
    if (it == adjacencyList.end()) {
        throw std::invalid_argument("getNeighbors: node " + std::to_string(nodeId) + " does not exist");
    }
    return it->second;
}

size_t Graph::edgeCount() const {
    size_t total = 0;
    for (const auto& [id, edges] : adjacencyList) {
        total += edges.size();
    }
    return total;
}

void Graph::clear() {
    nodes.clear();
    adjacencyList.clear();
}

// ─── Geometry ─────────────────────────────────────────────────────────────────

double Graph::haversineDistance(double lat1, double lon1,
                                double lat2, double lon2) {
    auto toRad = [](double deg) { return deg * PI / 180.0; };

    const double dlat = toRad(lat2 - lat1);
    const double dlon = toRad(lon2 - lon1);
    const double a = std::sin(dlat / 2.0) * std::sin(dlat / 2.0) +
                     std::cos(toRad(lat1)) * std::cos(toRad(lat2)) *
                     std::sin(dlon / 2.0) * std::sin(dlon / 2.0);
    const double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
    return EARTH_RADIUS_M * c;
}
