#ifndef OSMPARSER_H
#define OSMPARSER_H

#include "graph/Graph.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

// ─── Internal OSM structures ──────────────────────────────────────────────────

/// One <way> element extracted from the OSM file.
struct OSMWay {
    int64_t              id;
    std::vector<int64_t> nodeRefs;   // ordered node IDs forming the way
    bool                 isOneway  = false;
    std::string          highway;    // "motorway", "primary", "residential", …
    double               maxspeed  = 50.0;  // km/h (default urban)
};

// ─── Road-type speed defaults (km/h) ─────────────────────────────────────────
//
// Used when no maxspeed tag is present on a way.
//
inline const std::unordered_map<std::string, double> DEFAULT_SPEEDS = {
    {"motorway",       120.0},
    {"motorway_link",   60.0},
    {"trunk",          100.0},
    {"trunk_link",      50.0},
    {"primary",         80.0},
    {"primary_link",    50.0},
    {"secondary",       60.0},
    {"secondary_link",  50.0},
    {"tertiary",        50.0},
    {"tertiary_link",   40.0},
    {"unclassified",    40.0},
    {"residential",     30.0},
    {"living_street",   10.0},
    {"service",         20.0},
    {"track",           20.0},
    {"path",             5.0},
};

// ─── OSMParser class ──────────────────────────────────────────────────────────

class OSMParser {
public:
    /// Represents what was loaded after a parse call.
    struct ParseResult {
        size_t nodesParsed = 0;
        size_t edgesAdded  = 0;
        bool   success     = false;
        std::string errorMessage;
    };

    // ── Public API ───────────────────────────────────────────────────────────

    /// Parse an OSM XML (.osm) file and populate @p graph.
    /// Only routable highway ways are imported.
    /// @throws std::runtime_error on fatal I/O errors.
    static ParseResult parseOSMXML(const std::string& filePath, Graph& graph);

    /// Parse an OSM XML from an in-memory string (useful for unit tests).
    static ParseResult parseOSMXMLFromString(const std::string& xmlContent, Graph& graph);

private:
    // ── Internals ────────────────────────────────────────────────────────────

    /// Extract the value of @p attr from an XML tag line, e.g. id="123" → "123".
    static std::string getAttribute(const std::string& line, const std::string& attr);

    /// True if a highway tag value should be imported as a routable edge.
    static bool isRoutableHighway(const std::string& highwayType);

    /// Compute travel-time edge weight in seconds from a distance and speed.
    static double travelTimeSeconds(double distanceMetres, double speedKmh);

    /// Build graph edges from parsed nodes + ways.
    static size_t buildEdges(const std::unordered_map<int64_t, std::pair<double, double>>& nodeCoords,
                             const std::vector<OSMWay>& ways,
                             Graph& graph);

    /// Core SAX-style line-by-line XML parse shared by both public parse methods.
    static ParseResult parseStream(std::istream& stream, Graph& graph);
};

#endif // OSMPARSER_H
