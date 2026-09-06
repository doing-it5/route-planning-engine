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

if(MSVC)
    target_compile_options(run_tests PRIVATE /EHsc /W4)
endif()

add_test(NAME unit_tests COMMAND run_tests)
