#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "algorithms/HeldKarp.h"

class HeldKarpTest : public ::testing::Test {
protected:
    Graph graph;

    void SetUp() override {
        // Create a 4-node complete cycle graph:
        // 1 - 2 - 3 - 4 - 1
        graph.addNode(1, 0.0, 0.0);
        graph.addNode(2, 0.0, 1.0);
        graph.addNode(3, 1.0, 1.0);
        graph.addNode(4, 1.0, 0.0);

        auto addBiEdge = [&](int64_t u, int64_t v, double w) {
            graph.addEdge(u, v, w, w);
            graph.addEdge(v, u, w, w);
        };

        addBiEdge(1, 2, 10.0);
        addBiEdge(2, 3, 15.0);
        addBiEdge(3, 4, 20.0);
        addBiEdge(4, 1, 25.0);
        addBiEdge(1, 3, 35.0);
        addBiEdge(2, 4, 30.0);
    }
};

TEST_F(HeldKarpTest, FindsExactOptimalTour) {
    std::vector<int64_t> stops = {1, 2, 3, 4};
    auto result = HeldKarp::solve(graph, stops);

    EXPECT_TRUE(result.found);
    // Perimeter tour: 10 + 15 + 20 + 25 = 70.0
    EXPECT_DOUBLE_EQ(result.totalDistance, 70.0);
    EXPECT_EQ(result.tour.size(), static_cast<size_t>(4));
    EXPECT_EQ(result.tour[0], 1); // starts at stop 1
}

TEST_F(HeldKarpTest, ThrowsOnInvalidStopCount) {
    EXPECT_THROW(HeldKarp::solve(graph, {1}), std::invalid_argument);

    std::vector<int64_t> tooMany(21, 1);
    EXPECT_THROW(HeldKarp::solve(graph, tooMany), std::invalid_argument);
}
