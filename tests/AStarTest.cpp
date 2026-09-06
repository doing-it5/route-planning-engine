#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "algorithms/AStar.h"
#include "algorithms/Dijkstra.h"

class AStarTest : public ::testing::Test {
protected:
    Graph graph;

    void SetUp() override {
        // Build a grid graph with lat/lon coordinates
        // 1 (0,0) -> 2 (0,1) -> 3 (0,2)
        // 1 (0,0) -> 4 (1,0) -> 5 (1,1) -> 3 (0,2)
        graph.addNode(1, 40.0, -74.0);
        graph.addNode(2, 40.0, -73.9);
        graph.addNode(3, 40.0, -73.8);
        graph.addNode(4, 40.1, -74.0);
        graph.addNode(5, 40.1, -73.9);

        double d12 = Graph::haversineDistance(40.0, -74.0, 40.0, -73.9);
        double d23 = Graph::haversineDistance(40.0, -73.9, 40.0, -73.8);
        double d14 = Graph::haversineDistance(40.0, -74.0, 40.1, -74.0);
        double d45 = Graph::haversineDistance(40.1, -74.0, 40.1, -73.9);
        double d53 = Graph::haversineDistance(40.1, -73.9, 40.0, -73.8);

        graph.addEdge(1, 2, d12, d12);
        graph.addEdge(2, 3, d23, d23);
        graph.addEdge(1, 4, d14, d14);
        graph.addEdge(4, 5, d45, d45);
        graph.addEdge(5, 3, d53, d53);
    }
};

TEST_F(AStarTest, FindsShortestPathConsistentWithDijkstra) {
    auto astarRes = AStar::findShortest(graph, 1, 3);
    auto dijkRes  = Dijkstra::findShortest(graph, 1, 3);

    EXPECT_TRUE(astarRes.found);
    EXPECT_TRUE(dijkRes.found);
    EXPECT_NEAR(astarRes.distance, dijkRes.distance, 1e-5);
    EXPECT_EQ(astarRes.path, dijkRes.path);
}

TEST_F(AStarTest, StartEqualsEnd) {
    auto result = AStar::findShortest(graph, 2, 2);
    EXPECT_TRUE(result.found);
    EXPECT_DOUBLE_EQ(result.distance, 0.0);
    std::vector<int64_t> expected = {2};
    EXPECT_EQ(result.path, expected);
}

TEST_F(AStarTest, UnreachableNode) {
    graph.addNode(6, 45.0, -80.0);
    auto result = AStar::findShortest(graph, 1, 6);
    EXPECT_FALSE(result.found);
    EXPECT_TRUE(result.path.empty());
}
