#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "parser/OSMParser.h"

class OSMParserTest : public ::testing::Test {
protected:
    Graph graph;
};

TEST_F(OSMParserTest, ParsesSimpleOSMXML) {
    const std::string xml = R"(<?xml version="1.0" encoding="UTF-8"?>
<osm version="0.6">
  <node id="1" lat="40.7128" lon="-74.0060"/>
  <node id="2" lat="40.7580" lon="-73.9855"/>
  <way id="100">
    <nd ref="1"/>
    <nd ref="2"/>
    <tag k="highway" v="primary"/>
    <tag k="maxspeed" v="60"/>
  </way>
</osm>)";

    auto result = OSMParser::parseOSMXMLFromString(xml, graph);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.nodesParsed, 2u);
    // Bidirectional way = 2 edges
    EXPECT_EQ(result.edgesAdded, 2u);
    EXPECT_EQ(graph.nodeCount(), 2u);
    EXPECT_EQ(graph.edgeCount(), 2u);
}

TEST_F(OSMParserTest, HandlesOnewayTag) {
    const std::string xml = R"(<?xml version="1.0" encoding="UTF-8"?>
<osm version="0.6">
  <node id="10" lat="40.7128" lon="-74.0060"/>
  <node id="20" lat="40.7580" lon="-73.9855"/>
  <way id="101">
    <nd ref="10"/>
    <nd ref="20"/>
    <tag k="highway" v="motorway"/>
    <tag k="oneway" v="yes"/>
  </way>
</osm>)";

    auto result = OSMParser::parseOSMXMLFromString(xml, graph);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.edgesAdded, 1u);
    EXPECT_EQ(graph.getNeighbors(10).size(), 1u);
    EXPECT_EQ(graph.getNeighbors(20).size(), 0u);
}

TEST_F(OSMParserTest, IgnoresNonRoutableWays) {
    const std::string xml = R"(<?xml version="1.0" encoding="UTF-8"?>
<osm version="0.6">
  <node id="100" lat="40.0" lon="-74.0"/>
  <node id="200" lat="40.1" lon="-74.1"/>
  <way id="999">
    <nd ref="100"/>
    <nd ref="200"/>
    <tag k="building" v="yes"/>
  </way>
</osm>)";

    auto result = OSMParser::parseOSMXMLFromString(xml, graph);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.edgesAdded, 0u);
}
