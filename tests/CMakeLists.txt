set(TEST_SOURCES
    GraphTest.cpp
    DijkstraTest.cpp
    AStarTest.cpp
    HeldKarpTest.cpp
    TwoOptTest.cpp
    OSMParserTest.cpp
    BenchmarkTest.cpp
)

add_executable(run_tests ${TEST_SOURCES})
target_link_libraries(run_tests PRIVATE
    route_planning_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(run_tests)
