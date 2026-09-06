#include <gtest/gtest.h>
#include "algorithms/TwoOpt.h"
#include <vector>
#include <cmath>

class TwoOptTest : public ::testing::Test {};

TEST_F(TwoOptTest, SmallTourHandlesUnderFourStops) {
    std::vector<int64_t> tour = {1, 2, 3};
    std::vector<std::vector<double>> matrix = {
        {0, 5, 10},
        {5, 0, 8},
        {10, 8, 0}
    };
    auto result = TwoOpt::improve(tour, matrix);
    EXPECT_EQ(result.tour, tour);
    EXPECT_EQ(result.iterations, 0);
    // Perimeter cycle: (0->1: 5) + (1->2: 8) + (2->0: 10) = 23.0
    EXPECT_DOUBLE_EQ(result.totalDistance, 23.0);
}

TEST_F(TwoOptTest, UncrossesCrossingEdges) {
    // 4 corners of a square:
    // (0,0), (1,1), (0,1), (1,0)
    // Order 0 -> 1 -> 2 -> 3 -> 0 produces crossed diagonals (cost 2*sqrt(2) + 2 = ~4.828)
    // Optimal order 0 -> 2 -> 1 -> 3 -> 0 (square perimeter) has cost 4.0
    std::vector<int64_t> tour = {0, 1, 2, 3};
    std::vector<std::pair<double, double>> pts = {
        {0.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}, {1.0, 0.0}
    };

    std::vector<std::vector<double>> matrix(4, std::vector<double>(4, 0.0));
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            double dx = pts[i].first - pts[j].first;
            double dy = pts[i].second - pts[j].second;
            matrix[i][j] = std::sqrt(dx * dx + dy * dy);
        }
    }

    auto result = TwoOpt::improve(tour, matrix);
    EXPECT_GT(result.iterations, 0);
    EXPECT_NEAR(result.totalDistance, 4.0, 1e-5);
}
