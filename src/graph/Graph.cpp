#include "graph/Graph.h"

#include <cmath>
#include <stdexcept>
#include <algorithm>

static constexpr double PI = 3.14159265358979323846;
static constexpr double EARTH_RADIUS_M = 6371000.0;  // metres

// ─── Node operations ─────────────────────────────────────────────────────────

void Graph::addNode(int64_t id, double latitude, double longitude) {
    if (!nodeExists(id)) {
        nodes[id]         = Node(id, latitude, longitude);
        adjacencyList[id] = {};  // ensure entry exists even with no edges
        incomingAdjacencyList[id] = {};
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
    incomingAdjacencyList[to].emplace_back(from, to, weight, distance);
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

const std::vector<Edge>& Graph::getIncomingEdges(int64_t nodeId) const {
    auto it = incomingAdjacencyList.find(nodeId);
    if (it == incomingAdjacencyList.end()) {
        throw std::invalid_argument("getIncomingEdges: node " + std::to_string(nodeId) + " does not exist");
    }
    return it->second;
}

size_t Graph::edgeCount() const {
    size_t total = 0;
    for (const auto& kv : adjacencyList) {
        total += kv.second.size();
    }
    return total;
}

void Graph::clear() {
    nodes.clear();
    adjacencyList.clear();
    incomingAdjacencyList.clear();
}

void Graph::removeNode(int64_t id) {
    if (!nodeExists(id)) return;
    nodes.erase(id);
    adjacencyList.erase(id);
    incomingAdjacencyList.erase(id);
    
    // Remove all edges pointing TO this node from other nodes
    for (auto& kv : adjacencyList) {
        auto& edges = kv.second;
        edges.erase(
            std::remove_if(edges.begin(), edges.end(),
                           [id](const Edge& e) { return e.to == id; }),
            edges.end());
    }
    // Remove all edges pointing FROM this node in incomingAdjacencyList
    for (auto& kv : incomingAdjacencyList) {
        auto& edges = kv.second;
        edges.erase(
            std::remove_if(edges.begin(), edges.end(),
                           [id](const Edge& e) { return e.from == id; }),
            edges.end());
    }
}

void Graph::removeEdge(int64_t from, int64_t to) {
    auto it = adjacencyList.find(from);
    if (it != adjacencyList.end()) {
        auto& edges = it->second;
        edges.erase(
            std::remove_if(edges.begin(), edges.end(),
                           [to](const Edge& e) { return e.to == to; }),
            edges.end());
    }
    auto it_in = incomingAdjacencyList.find(to);
    if (it_in != incomingAdjacencyList.end()) {
        auto& edges = it_in->second;
        edges.erase(
            std::remove_if(edges.begin(), edges.end(),
                           [from](const Edge& e) { return e.from == from; }),
            edges.end());
    }
}

// ─── Geometry ─────────────────────────────────────────────────────────────────

double Graph::haversineDistance(double lat1, double lon1,
                                double lat2, double lon2) {
    double dLat = (lat2 - lat1) * PI / 180.0;
    double dLon = (lon2 - lon1) * PI / 180.0;
    lat1 = lat1 * PI / 180.0;
    lat2 = lat2 * PI / 180.0;
    double a = std::pow(std::sin(dLat / 2), 2) +
               std::pow(std::sin(dLon / 2), 2) * std::cos(lat1) * std::cos(lat2);
    a = std::min(1.0, std::max(0.0, a));
    double c = 2 * std::asin(std::sqrt(a));
    return EARTH_RADIUS_M * c;
}
