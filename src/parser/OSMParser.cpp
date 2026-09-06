#include "parser/OSMParser.h"
#include "graph/Graph.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <cctype>

// ─── Routable highway types ───────────────────────────────────────────────────

bool OSMParser::isRoutableHighway(const std::string& hw) {
    static const std::vector<std::string> ROUTABLE = {
        "motorway", "motorway_link",
        "trunk", "trunk_link",
        "primary", "primary_link",
        "secondary", "secondary_link",
        "tertiary", "tertiary_link",
        "unclassified",
        "residential",
        "living_street",
        "service",
        "track",
    };
    for (const auto& r : ROUTABLE) {
        if (hw == r) return true;
    }
    return false;
}

// ─── Travel-time weight ───────────────────────────────────────────────────────

double OSMParser::travelTimeSeconds(double distanceMetres, double speedKmh) {
    if (speedKmh <= 0.0) speedKmh = 30.0;
    // distance [m] / (speed [m/s]) = time [s]
    return distanceMetres / (speedKmh / 3.6);
}

// ─── Attribute extraction (no regex — fast and portable) ─────────────────────
//
// Handles both   attr="value"   and   attr='value'
//
std::string OSMParser::getAttribute(const std::string& line, const std::string& attr) {
    // Search for   attr="   or   attr='
    std::string needle1 = attr + "=\"";
    std::string needle2 = attr + "='";

    size_t pos = line.find(needle1);
    char   close = '"';
    if (pos == std::string::npos) {
        pos   = line.find(needle2);
        close = '\'';
    }
    if (pos == std::string::npos) return "";

    pos += attr.size() + 2;  // skip  attr="
    size_t end = line.find(close, pos);
    if (end == std::string::npos) return "";

    return line.substr(pos, end - pos);
}

// ─── Build edges from parsed data ────────────────────────────────────────────

size_t OSMParser::buildEdges(
    const std::unordered_map<int64_t, std::pair<double, double>>& nodeCoords,
    const std::vector<OSMWay>& ways,
    Graph& graph)
{
    size_t edgesAdded = 0;

    for (const OSMWay& way : ways) {
        if (way.nodeRefs.size() < 2) continue;

        // Determine speed for this way
        double speed = way.maxspeed;
        if (speed <= 0.0) {
            auto it = DEFAULT_SPEEDS.find(way.highway);
            speed = (it != DEFAULT_SPEEDS.end()) ? it->second : 50.0;
        }

        for (size_t i = 0; i + 1 < way.nodeRefs.size(); ++i) {
            const int64_t fromId = way.nodeRefs[i];
            const int64_t toId   = way.nodeRefs[i + 1];

            // Both endpoints must be in the graph
            if (!graph.nodeExists(fromId) || !graph.nodeExists(toId)) continue;

            const auto& fc = nodeCoords.at(fromId);
            const auto& tc = nodeCoords.at(toId);

            const double dist   = Graph::haversineDistance(fc.first, fc.second,
                                                            tc.first, tc.second);
            const double weight = travelTimeSeconds(dist, speed);  // seconds

            // Forward edge
            graph.addEdge(fromId, toId, weight, dist);
            ++edgesAdded;

            // Reverse edge (unless one-way)
            if (!way.isOneway) {
                graph.addEdge(toId, fromId, weight, dist);
                ++edgesAdded;
            }
        }
    }

    return edgesAdded;
}

// ─── Core streaming parser ────────────────────────────────────────────────────
//
// SAX-style, line-by-line — works on arbitrarily large OSM XML files without
// loading the whole document into memory.
//
// Handles multi-line <node …/> and <way>…</way> elements by accumulating
// continuation lines until the closing tag is seen.
//
OSMParser::ParseResult OSMParser::parseStream(std::istream& stream, Graph& graph) {
    ParseResult pr;
    pr.success = false;

    std::unordered_map<int64_t, std::pair<double, double>> nodeCoords;  // id → (lat, lon)
    std::vector<OSMWay> ways;

    // ── State machine variables ───────────────────────────────────────────
    bool    inWay         = false;
    OSMWay  currentWay;
    std::string accumulated;  // buffer for multi-line elements

    auto processNode = [&](const std::string& tag) {
        const std::string id_s  = getAttribute(tag, "id");
        const std::string lat_s = getAttribute(tag, "lat");
        const std::string lon_s = getAttribute(tag, "lon");
        if (id_s.empty() || lat_s.empty() || lon_s.empty()) return;

        try {
            int64_t id  = std::stoll(id_s);
            double  lat = std::stod(lat_s);
            double  lon = std::stod(lon_s);
            nodeCoords[id] = {lat, lon};
            graph.addNode(id, lat, lon);
            ++pr.nodesParsed;
        } catch (...) { /* malformed — skip */ }
    };

    auto processNd = [&](const std::string& tag) {
        const std::string ref_s = getAttribute(tag, "ref");
        if (!ref_s.empty()) {
            try {
                currentWay.nodeRefs.push_back(std::stoll(ref_s));
            } catch (...) {}
        }
    };

    auto processTag = [&](const std::string& tag) {
        const std::string k = getAttribute(tag, "k");
        const std::string v = getAttribute(tag, "v");
        if (k.empty() || v.empty()) return;

        if (k == "highway") {
            currentWay.highway  = v;
        } else if (k == "oneway") {
            currentWay.isOneway = (v == "yes" || v == "1" || v == "true");
        } else if (k == "maxspeed") {
            // value like "50", "50 mph", "none", "walk"
            try {
                double s = std::stod(v);
                // Convert mph to km/h
                if (v.find("mph") != std::string::npos) s *= 1.60934;
                currentWay.maxspeed = s;
            } catch (...) {
                // non-numeric values ("none", "walk") → keep default
            }
        }
    };

    // ── Line-by-line scan ─────────────────────────────────────────────────
    std::string line;
    while (std::getline(stream, line)) {
        // Trim leading whitespace for easier matching
        size_t start = line.find_first_not_of(" \t\r");
        const std::string& trimmed = (start == std::string::npos)
                                     ? line
                                     : line.substr(start);

        // ── <node … /> ─────────────────────────────────────────────────
        if (!inWay && trimmed.find("<node") == 0) {
            if (trimmed.find("/>") != std::string::npos) {
                processNode(trimmed);
            } else {
                // Multi-line node (rare) — accumulate until />
                accumulated = trimmed;
            }
            continue;
        }

        // Handle accumulated multi-line node
        if (!accumulated.empty()) {
            accumulated += " " + trimmed;
            if (trimmed.find("/>") != std::string::npos || trimmed.find("</node>") != std::string::npos) {
                processNode(accumulated);
                accumulated.clear();
            }
            continue;
        }

        // ── <way …> ────────────────────────────────────────────────────
        if (!inWay && trimmed.find("<way") == 0) {
            inWay       = true;
            currentWay  = OSMWay{};
            const std::string id_s = getAttribute(trimmed, "id");
            try { currentWay.id = std::stoll(id_s); } catch (...) {}
            continue;
        }

        // ── Inside a <way> ─────────────────────────────────────────────
        if (inWay) {
            if (trimmed.find("<nd") == 0) {
                processNd(trimmed);
            } else if (trimmed.find("<tag") == 0) {
                processTag(trimmed);
            } else if (trimmed.find("</way>") != std::string::npos) {
                inWay = false;
                if (isRoutableHighway(currentWay.highway)) {
                    ways.push_back(currentWay);
                }
            }
        }
    }

    // ── Build edges ───────────────────────────────────────────────────────
    pr.edgesAdded = buildEdges(nodeCoords, ways, graph);
    pr.success    = true;
    return pr;
}

// ─── Public API ───────────────────────────────────────────────────────────────

OSMParser::ParseResult OSMParser::parseOSMXML(const std::string& filePath, Graph& graph) {
    ParseResult pr;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        pr.errorMessage = "Cannot open file: " + filePath;
        return pr;
    }

    std::cout << "[OSMParser] Parsing: " << filePath << "\n";
    pr = parseStream(file, graph);

    if (pr.success) {
        std::cout << "[OSMParser] Done — nodes: " << pr.nodesParsed
                  << "  edges: " << pr.edgesAdded << "\n";
    }
    return pr;
}

OSMParser::ParseResult OSMParser::parseOSMXMLFromString(const std::string& xmlContent,
                                                         Graph& graph) {
    std::istringstream ss(xmlContent);
    return parseStream(ss, graph);
}
