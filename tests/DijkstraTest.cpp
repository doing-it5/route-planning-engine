#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "algorithms/Dijkstra.h"

class DijkstraTest : public ::testing::Test {
protected:
    Graph graph;

    void SetUp() override {
        // Build a 4-node diamond graph:
        // 1 -> 2 (weight 1) -> 4 (weight 1)
        // 1 -> 3 (weight 4) -> 4 (weight 1)
        graph.addNode(1, 40.0, -74.0);
        graph.addNode(2, 40.1, -74.0);
        graph.addNode(3, 40.0, -73.9);
        graph.addNode(4, 40.1, -73.9);

        graph.addEdge(1, 2, 1.0, 100.0);
        graph.addEdge(2, 4, 1.0, 100.0);
        graph.addEdge(1, 3, 4.0, 400.0);
        graph.addEdge(3, 4, 1.0, 100.0);
    }
};

TEST_F(DijkstraTest, FindsOptimalShortestPath) {
    auto result = Dijkstra::findShortest(graph, 1, 4);
    EXPECT_TRUE(result.found);
    EXPECT_DOUBLE_EQ(result.distance, 2.0);
    std::vector<int64_t> expectedPath = {1, 2, 4};
    EXPECT_EQ(result.path, expectedPath);
    EXPECT_GT(result.nodeExpansions, 0);
}

TEST_F(DijkstraTest, StartEqualsEnd) {
    auto result = Dijkstra::findShortest(graph, 1, 1);
    EXPECT_TRUE(result.found);
    EXPECT_DOUBLE_EQ(result.distance, 0.0);
    std::vector<int64_t> expectedPath = {1};
    EXPECT_EQ(result.path, expectedPath);
}

TEST_F(DijkstraTest, UnreachableNode) {
    graph.addNode(5, 41.0, -75.0); // disconnected node
    auto result = Dijkstra::findShortest(graph, 1, 5);
    EXPECT_FALSE(result.found);
    EXPECT_TRUE(result.path.empty());
}

TEST_F(DijkstraTest, NonExistentNodes) {
    auto result = Dijkstra::findShortest(graph, 999, 4);
    EXPECT_FALSE(result.found);

    auto result2 = Dijkstra::findShortest(graph, 1, 999);
    EXPECT_FALSE(result2.found);
}
