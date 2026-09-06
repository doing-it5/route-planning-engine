#include <gtest/gtest.h>
#include "graph/Graph.h"

class GraphTest : public ::testing::Test {
protected:
    Graph graph;
};

TEST_F(GraphTest, AddNodeAndQuery) {
    graph.addNode(1, 40.7128, -74.0060);
    EXPECT_TRUE(graph.nodeExists(1));
    EXPECT_FALSE(graph.nodeExists(2));

    const Node* n = graph.getNode(1);
    ASSERT_NE(n, nullptr);
    EXPECT_EQ(n->id, 1);
    EXPECT_DOUBLE_EQ(n->latitude, 40.7128);
    EXPECT_DOUBLE_EQ(n->longitude, -74.0060);
}

TEST_F(GraphTest, AddEdgeAndNeighbors) {
    graph.addNode(1, 40.7128, -74.0060);
    graph.addNode(2, 40.7580, -73.9855);
    graph.addEdge(1, 2, 5.0, 5000.0);

    EXPECT_EQ(graph.nodeCount(), 2u);
    EXPECT_EQ(graph.edgeCount(), 1u);

    const auto& neighbors = graph.getNeighbors(1);
    ASSERT_EQ(neighbors.size(), 1u);
    EXPECT_EQ(neighbors[0].from, 1);
    EXPECT_EQ(neighbors[0].to, 2);
    EXPECT_DOUBLE_EQ(neighbors[0].weight, 5.0);
    EXPECT_DOUBLE_EQ(neighbors[0].distance, 5000.0);
}

TEST_F(GraphTest, HaversineDistanceCalculation) {
    // Distance between NYC (Times Square) and Empire State Building is approx 900-1000m
    double dist = Graph::haversineDistance(40.7580, -73.9855, 40.7484, -73.9857);
    EXPECT_GT(dist, 900.0);
    EXPECT_LT(dist, 1200.0);

    // Distance to same point is 0
    EXPECT_DOUBLE_EQ(Graph::haversineDistance(40.0, -74.0, 40.0, -74.0), 0.0);
}

TEST_F(GraphTest, ClearGraph) {
    graph.addNode(1, 10.0, 20.0);
    graph.addNode(2, 10.1, 20.1);
    graph.addEdge(1, 2, 1.0, 100.0);
    EXPECT_EQ(graph.nodeCount(), 2u);

    graph.clear();
    EXPECT_EQ(graph.nodeCount(), 0u);
    EXPECT_EQ(graph.edgeCount(), 0u);
}
