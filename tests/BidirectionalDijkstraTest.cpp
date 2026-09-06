#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "algorithms/BidirectionalDijkstra.h"
#include "algorithms/Dijkstra.h"

class BiDijkstraTest : public ::testing::Test {
protected:
    Graph graph;

    void SetUp() override {
        // Simple 5-node bidirectional graph
        graph.addNode(1, 40.0, -74.0);
        graph.addNode(2, 40.1, -74.0);
        graph.addNode(3, 40.2, -74.0);
        graph.addNode(4, 40.1, -73.9);
        graph.addNode(5, 40.2, -73.9);

        graph.addEdge(1, 2, 10.0, 10000.0);
        graph.addEdge(2, 1, 10.0, 10000.0);
        graph.addEdge(2, 3, 10.0, 10000.0);
        graph.addEdge(3, 2, 10.0, 10000.0);
        graph.addEdge(2, 4,  5.0,  5000.0);
        graph.addEdge(4, 2,  5.0,  5000.0);
        graph.addEdge(4, 5,  5.0,  5000.0);
        graph.addEdge(5, 4,  5.0,  5000.0);
        graph.addEdge(3, 5,  5.0,  5000.0);
        graph.addEdge(5, 3,  5.0,  5000.0);
    }
};

TEST_F(BiDijkstraTest, MatchesUnidirectionalDijkstra) {
    auto biRes  = BidirectionalDijkstra::findShortest(graph, 1, 5);
    auto uniRes = Dijkstra::findShortest(graph, 1, 5);

    EXPECT_TRUE(biRes.found);
    EXPECT_TRUE(uniRes.found);
    EXPECT_NEAR(biRes.distance, uniRes.distance, 1e-9);
}

TEST_F(BiDijkstraTest, StartEqualsEnd) {
    auto result = BidirectionalDijkstra::findShortest(graph, 3, 3);
    EXPECT_TRUE(result.found);
    EXPECT_DOUBLE_EQ(result.distance, 0.0);
    ASSERT_EQ(result.path.size(), static_cast<size_t>(1));
    EXPECT_EQ(result.path[0], 3);
}

TEST_F(BiDijkstraTest, UnreachableNode) {
    graph.addNode(99, 50.0, -80.0);  // isolated node
    auto result = BidirectionalDijkstra::findShortest(graph, 1, 99);
    EXPECT_FALSE(result.found);
}

TEST_F(BiDijkstraTest, NodeDoesNotExist) {
    auto result = BidirectionalDijkstra::findShortest(graph, 1, 999);
    EXPECT_FALSE(result.found);
}
