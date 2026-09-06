set(TEST_SOURCES
    GraphTest.cpp
    DijkstraTest.cpp
    AStarTest.cpp
    HeldKarpTest.cpp
    TwoOptTest.cpp
    OSMParserTest.cpp
    BenchmarkTest.cpp
)

add_executable(unit_tests_exe ${TEST_SOURCES})
target_link_libraries(unit_tests_exe PRIVATE
    route_planning_lib
    GTest::gtest_main
)

if(MSVC)
    target_compile_options(unit_tests_exe PRIVATE /EHsc /W4)
endif()

add_test(NAME unit_tests COMMAND $<TARGET_FILE:unit_tests_exe>)
