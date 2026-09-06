#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "algorithms/Dijkstra.h"
#include "algorithms/AStar.h"

// ─── Extra edge-case tests ────────────────────────────────────────────────────

TEST(GraphEdgeCases, SingleNodeGraph) {
    Graph g;
    g.addNode(1, 0.0, 0.0);
    EXPECT_EQ(g.nodeCount(), static_cast<size_t>(1));
    EXPECT_EQ(g.edgeCount(), static_cast<size_t>(0));

    auto res = Dijkstra::findShortest(g, 1, 1);
    EXPECT_TRUE(res.found);
    EXPECT_DOUBLE_EQ(res.distance, 0.0);
}

TEST(GraphEdgeCases, RemoveNode) {
    Graph g;
    g.addNode(1, 0.0, 0.0);
    g.addNode(2, 0.1, 0.1);
    g.addEdge(1, 2, 1.0, 100.0);
    g.addEdge(2, 1, 1.0, 100.0);

    EXPECT_EQ(g.nodeCount(), static_cast<size_t>(2));
    g.removeNode(2);
    EXPECT_EQ(g.nodeCount(), static_cast<size_t>(1));
    EXPECT_EQ(g.edgeCount(), static_cast<size_t>(0));  // edge 1→2 removed too
    EXPECT_FALSE(g.nodeExists(2));
}

TEST(GraphEdgeCases, RemoveNodeNoop) {
    Graph g;
    g.addNode(1, 0.0, 0.0);
    g.removeNode(999);  // should not throw or crash
    EXPECT_EQ(g.nodeCount(), static_cast<size_t>(1));
}

TEST(GraphEdgeCases, RemoveEdge) {
    Graph g;
    g.addNode(1, 0.0, 0.0);
    g.addNode(2, 0.1, 0.1);
    g.addEdge(1, 2, 1.0, 100.0);
    EXPECT_EQ(g.edgeCount(), static_cast<size_t>(1));

    g.removeEdge(1, 2);
    EXPECT_EQ(g.edgeCount(), static_cast<size_t>(0));
}

TEST(GraphEdgeCases, RemoveEdgeNoop) {
    Graph g;
    g.addNode(1, 0.0, 0.0);
    g.removeEdge(1, 999);  // should not throw or crash
    EXPECT_EQ(g.edgeCount(), static_cast<size_t>(0));
}

TEST(DijkstraEdgeCases, CyclicGraph) {
    Graph g;
    g.addNode(1, 0.0, 0.0);
    g.addNode(2, 0.1, 0.0);
    g.addNode(3, 0.2, 0.0);
    g.addEdge(1, 2, 1.0, 100.0);
    g.addEdge(2, 3, 1.0, 100.0);
    g.addEdge(3, 1, 1.0, 100.0);  // cycle

    auto res = Dijkstra::findShortest(g, 1, 3);
    EXPECT_TRUE(res.found);
    EXPECT_NEAR(res.distance, 2.0, 1e-9);
}

TEST(DijkstraEdgeCases, NonExistentStartNode) {
    Graph g;
    g.addNode(1, 0.0, 0.0);
    auto res = Dijkstra::findShortest(g, 999, 1);
    EXPECT_FALSE(res.found);
}

TEST(AStarEdgeCases, CyclicGraph) {
    Graph g;
    g.addNode(1, 40.0, -74.0);
    g.addNode(2, 40.1, -74.0);
    g.addNode(3, 40.2, -74.0);
    double d12 = Graph::haversineDistance(40.0, -74.0, 40.1, -74.0);
    double d23 = Graph::haversineDistance(40.1, -74.0, 40.2, -74.0);
    double d31 = Graph::haversineDistance(40.2, -74.0, 40.0, -74.0);
    g.addEdge(1, 2, d12, d12);
    g.addEdge(2, 3, d23, d23);
    g.addEdge(3, 1, d31, d31);  // cycle

    auto res = AStar::findShortest(g, 1, 3);
    EXPECT_TRUE(res.found);
}

TEST(AStarEdgeCases, NonExistentEndNode) {
    Graph g;
    g.addNode(1, 40.0, -74.0);
    auto res = AStar::findShortest(g, 1, 999);
    EXPECT_FALSE(res.found);
}

TEST(OSMParserEdgeCases, SelfClosingWayAndEmptyXML) {
    Graph g;
    const std::string xml = R"(<?xml version="1.0" encoding="UTF-8"?>
<osm version="0.6">
  <node id="1" lat="40.0" lon="-74.0"/>
  <node id="2" lat="40.1" lon="-74.0"/>
  <way id="99" />
  <way id="100">
    <nd ref="1"/>
    <nd ref="2"/>
    <tag k="highway" v="primary"/>
  </way>
</osm>)";
    auto res = OSMParser::parseOSMXMLFromString(xml, g);
    EXPECT_TRUE(res.success);
    EXPECT_EQ(g.nodeCount(), static_cast<size_t>(2));
    EXPECT_EQ(g.edgeCount(), static_cast<size_t>(2));
}
