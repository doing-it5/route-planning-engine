/**
 * route-planning-engine  —  main.cpp
 *
 * CLI Usage:
 *   ./route_planner --osm <file.osm> --from <nodeId> --to <nodeId> [--algo dijkstra|astar]
 *   ./route_planner --osm <file.osm> --stops <id1,id2,id3,...> [--algo tsp]
 *   ./route_planner --demo          # run on a built-in synthetic graph
 *
 * Options:
 *   --osm    <path>   Path to an OSM XML (.osm) file
 *   --from   <id>     Source node ID (for point-to-point routing)
 *   --to     <id>     Destination node ID
 *   --stops  <ids>    Comma-separated node IDs (for TSP multi-stop routing)
 *   --algo   <name>   Algorithm: dijkstra (default), astar, tsp
 *   --bench           Also run benchmarks after solving
 *   --demo            Ignore other flags and run the built-in demo
 */

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

#include "graph/Graph.h"
#include "algorithms/Dijkstra.h"
#include "algorithms/AStar.h"
#include "algorithms/BidirectionalDijkstra.h"
#include "algorithms/HeldKarp.h"
#include "algorithms/TwoOpt.h"
#include "parser/OSMParser.h"
#include "benchmark/Benchmark.h"
#include "utils/Utils.h"

// ─── Argument parser ──────────────────────────────────────────────────────────

struct Args {
    std::string osmFile;
    int64_t     fromNode = -1;
    int64_t     toNode   = -1;
    std::vector<int64_t> stops;
    std::string algo     = "dijkstra";  // dijkstra | astar | bidijkstra | tsp
    bool        bench    = false;
    bool        demo     = false;
};

static void printHelp(const char* prog) {
    std::cout << "Usage:\n"
              << "  " << prog << " --osm <file.osm> --from <id> --to <id> [--algo dijkstra|astar|bidijkstra]\n"
              << "  " << prog << " --osm <file.osm> --stops <id1,id2,...> [--algo tsp]\n"
              << "  " << prog << " --demo\n"
              << "\nOptions:\n"
              << "  --osm   <path>  OSM XML file to load\n"
              << "  --from  <id>    Source node ID\n"
              << "  --to    <id>    Destination node ID\n"
              << "  --stops <ids>   Comma-separated stop IDs for TSP\n"
              << "  --algo  <name>  dijkstra (default) | astar | bidijkstra | tsp\n"
              << "  --bench         Run benchmarks after solving\n"
              << "  --demo          Run built-in synthetic demo\n";
}

static Args parseArgs(int argc, char* argv[]) {
    Args args;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--help" || a == "-h") {
            printHelp(argv[0]);
            std::exit(0);
        } else if (a == "--demo")  { args.demo  = true; }
        else if (a == "--bench")   { args.bench = true; }
        else if (i + 1 < argc) {
            std::string v = argv[i + 1];
            if      (a == "--osm")   { args.osmFile = v;                         ++i; }
            else if (a == "--from")  { args.fromNode = std::stoll(v);            ++i; }
            else if (a == "--to")    { args.toNode   = std::stoll(v);            ++i; }
            else if (a == "--algo")  { args.algo     = v;                        ++i; }
            else if (a == "--stops") { args.stops = utils::parseNodeIdList(v);   ++i; }
        }
    }
    return args;
}

// ─── Demo on synthetic graph ──────────────────────────────────────────────────

static void runDemo() {
    std::cout << "=== Built-in Demo (synthetic NYC-area graph) ===\n\n";

    Graph graph;

    // Nodes with real lat/lon coordinates
    graph.addNode(1, 40.7128, -74.0060);   // Lower Manhattan
    graph.addNode(2, 40.7614, -73.9776);   // Midtown
    graph.addNode(3, 40.7489, -73.9680);   // Central Park South
    graph.addNode(4, 40.7505, -73.9934);   // Hell's Kitchen
    graph.addNode(5, 40.7282, -73.7949);   // Jamaica, Queens
    graph.addNode(6, 40.6892, -74.0445);   // Staten Island Ferry

    // Compute haversine distances and add edges
    auto addBiEdge = [&](int64_t a, int64_t b) {
        const Node* na = graph.getNode(a);
        const Node* nb = graph.getNode(b);
        double d = Graph::haversineDistance(na->latitude, na->longitude,
                                            nb->latitude, nb->longitude);
        double travelTime = d / (50.0 / 3.6);  // assume 50 km/h
        graph.addEdge(a, b, travelTime, d);
        graph.addEdge(b, a, travelTime, d);
    };

    addBiEdge(1, 2);
    addBiEdge(2, 3);
    addBiEdge(3, 4);
    addBiEdge(4, 1);
    addBiEdge(1, 6);
    addBiEdge(2, 4);
    addBiEdge(3, 5);

    std::cout << "Graph: " << graph.nodeCount() << " nodes, "
              << graph.edgeCount() << " edges\n\n";

    // ── Dijkstra ─────────────────────────────────────────────────────────
    std::cout << "--- Dijkstra: node 1 → node 5 ---\n";
    DijkstraResult dres = Dijkstra::findShortest(graph, 1, 5);
    if (dres.found) {
        double distM = 0.0;
        for (size_t i = 0; i + 1 < dres.path.size(); ++i) {
            for (const auto& e : graph.getNeighbors(dres.path[i])) {
                if (e.to == dres.path[i + 1]) { distM += e.distance; break; }
            }
        }
        std::cout << "  Distance  : " << utils::formatDistance(distM) << "\n";
        std::cout << "  Duration  : " << utils::formatDuration(dres.distance) << "\n";
        std::cout << "  Path      : ";
        for (int64_t id : dres.path) std::cout << id << " ";
        std::cout << "\n  Expansions: " << dres.nodeExpansions << "\n\n";
    } else {
        std::cout << "  No path found.\n\n";
    }

    // ── A* ───────────────────────────────────────────────────────────────
    std::cout << "--- A*: node 1 → node 5 ---\n";
    AStarResult ares = AStar::findShortest(graph, 1, 5);
    if (ares.found) {
        double distM = 0.0;
        for (size_t i = 0; i + 1 < ares.path.size(); ++i) {
            for (const auto& e : graph.getNeighbors(ares.path[i])) {
                if (e.to == ares.path[i + 1]) { distM += e.distance; break; }
            }
        }
        std::cout << "  Distance  : " << utils::formatDistance(distM) << "\n";
        std::cout << "  Duration  : " << utils::formatDuration(ares.distance) << "\n";
        std::cout << "  Path      : ";
        for (int64_t id : ares.path) std::cout << id << " ";
        std::cout << "\n  Expansions: " << ares.nodeExpansions << "\n\n";
    } else {
        std::cout << "  No path found.\n\n";
    }

    // ── Bidirectional Dijkstra ───────────────────────────────────────────
    std::cout << "--- Bidirectional Dijkstra: node 1 → node 5 ---\n";
    BiDijkstraResult bres = BidirectionalDijkstra::findShortest(graph, 1, 5);
    if (bres.found) {
        double distM = 0.0;
        for (size_t i = 0; i + 1 < bres.path.size(); ++i) {
            for (const auto& e : graph.getNeighbors(bres.path[i])) {
                if (e.to == bres.path[i + 1]) { distM += e.distance; break; }
            }
        }
        std::cout << "  Distance  : " << utils::formatDistance(distM) << "\n";
        std::cout << "  Duration  : " << utils::formatDuration(bres.distance) << "\n";
        std::cout << "  Path      : ";
        for (int64_t id : bres.path) std::cout << id << " ";
        std::cout << "\n  Expansions: " << bres.nodeExpansions << "\n\n";
    } else {
        std::cout << "  No path found.\n\n";
    }

    // ── TSP (HeldKarp) ────────────────────────────────────────────────────
    std::cout << "--- TSP (Held-Karp) over stops {1, 2, 3, 4} ---\n";
    HeldKarpResult hres = HeldKarp::solve(graph, {1, 2, 3, 4});
    if (hres.found) {
        std::cout << "  Total time: " << utils::formatDuration(hres.totalDistance) << "\n";
        std::cout << "  Tour      : ";
        for (int64_t id : hres.tour) std::cout << id << " ";
        std::cout << "\n\n";
    } else {
        std::cout << "  No TSP tour found.\n\n";
    }

    // ── Benchmark ─────────────────────────────────────────────────────────
    std::cout << "--- Benchmarks (100 iterations each) ---\n";
    std::vector<BenchmarkResult> brs;
    brs.push_back(Benchmark::run("Dijkstra 1→5",     [&]{ Dijkstra::findShortest(graph, 1, 5);              }, 100));
    brs.push_back(Benchmark::run("A* 1→5",           [&]{ AStar::findShortest(graph, 1, 5);                 }, 100));
    brs.push_back(Benchmark::run("BiDijkstra 1→5",   [&]{ BidirectionalDijkstra::findShortest(graph, 1, 5); }, 100));
    Benchmark::printResults(brs);
}

// ─── OSM-backed routing ───────────────────────────────────────────────────────

static void runOSMRouting(const Args& args) {
    Graph graph;

    // Load OSM file
    std::cout << "[INFO] Loading OSM file: " << args.osmFile << "\n";
    auto pr = OSMParser::parseOSMXML(args.osmFile, graph);
    if (!pr.success) {
        std::cerr << "[ERROR] " << pr.errorMessage << "\n";
        std::exit(1);
    }
    std::cout << "[INFO] Graph loaded: " << graph.nodeCount() << " nodes, "
              << graph.edgeCount() << " edges\n\n";

    // ── TSP mode ─────────────────────────────────────────────────────────
    if (args.algo == "tsp" || !args.stops.empty()) {
        std::vector<int64_t> stops = args.stops;
        if (stops.size() < 2) {
            std::cerr << "[ERROR] TSP requires at least 2 --stops\n";
            std::exit(1);
        }

        auto bench = Benchmark::run("HeldKarp TSP", [&]{
            HeldKarp::solve(graph, stops);
        });

        HeldKarpResult hres = HeldKarp::solve(graph, stops);
        if (hres.found) {
            std::cout << "TSP optimal tour:\n";
            std::cout << "  Total time : " << utils::formatDuration(hres.totalDistance) << "\n";
            std::cout << "  Tour       : ";
            for (int64_t id : hres.tour) std::cout << id << " ";
            std::cout << "\n";
        } else {
            std::cout << "[WARN] No valid tour found — check that all stops are connected.\n";
        }

        if (args.bench) {
            Benchmark::printResults({bench});
        }
        return;
    }

    // ── Point-to-point mode ───────────────────────────────────────────────
    if (args.fromNode < 0 || args.toNode < 0) {
        std::cerr << "[ERROR] Specify --from and --to for point-to-point routing.\n";
        printHelp("route_planner");
        std::exit(1);
    }

    if (!graph.nodeExists(args.fromNode)) {
        std::cerr << "[ERROR] Node " << args.fromNode << " not found in graph.\n";
        std::exit(1);
    }
    if (!graph.nodeExists(args.toNode)) {
        std::cerr << "[ERROR] Node " << args.toNode << " not found in graph.\n";
        std::exit(1);
    }

    auto printPath = [&](const std::vector<int64_t>& path, double travelSecs) {
        double distM = 0.0;
        for (size_t i = 0; i + 1 < path.size(); ++i) {
            for (const auto& e : graph.getNeighbors(path[i])) {
                if (e.to == path[i + 1]) { distM += e.distance; break; }
            }
        }
        std::cout << "  Distance   : " << utils::formatDistance(distM)         << "\n";
        std::cout << "  Est. time  : " << utils::formatDuration(travelSecs)    << "\n";
        std::cout << "  Hops       : " << (path.empty() ? 0 : path.size() - 1) << "\n";
        std::cout << "  Path       : ";
        for (int64_t id : path) std::cout << id << " ";
        std::cout << "\n";
    };

    if (args.algo == "astar") {
        std::cout << "--- A*: " << args.fromNode << " → " << args.toNode << " ---\n";
        auto bench = Benchmark::run("A*", [&]{ AStar::findShortest(graph, args.fromNode, args.toNode); });
        AStarResult res = AStar::findShortest(graph, args.fromNode, args.toNode);
        if (res.found) {
            printPath(res.path, res.distance);
            std::cout << "  Expansions : " << res.nodeExpansions << "\n";
        } else {
            std::cout << "  No path found.\n";
        }
        if (args.bench) Benchmark::printResults({bench});
    } else if (args.algo == "bidijkstra") {
        std::cout << "--- Bidirectional Dijkstra: " << args.fromNode << " → " << args.toNode << " ---\n";
        auto bench = Benchmark::run("BiDijkstra", [&]{ BidirectionalDijkstra::findShortest(graph, args.fromNode, args.toNode); });
        BiDijkstraResult res = BidirectionalDijkstra::findShortest(graph, args.fromNode, args.toNode);
        if (res.found) {
            printPath(res.path, res.distance);
            std::cout << "  Expansions : " << res.nodeExpansions << "\n";
        } else {
            std::cout << "  No path found.\n";
        }
        if (args.bench) Benchmark::printResults({bench});
    } else {
        // Default: Dijkstra
        std::cout << "--- Dijkstra: " << args.fromNode << " → " << args.toNode << " ---\n";
        auto bench = Benchmark::run("Dijkstra", [&]{ Dijkstra::findShortest(graph, args.fromNode, args.toNode); });
        DijkstraResult res = Dijkstra::findShortest(graph, args.fromNode, args.toNode);
        if (res.found) {
            printPath(res.path, res.distance);
            std::cout << "  Expansions : " << res.nodeExpansions << "\n";
        } else {
            std::cout << "  No path found.\n";
        }
        if (args.bench) Benchmark::printResults({bench});
    }
}

// ─── Entry point ─────────────────────────────────────────────────────────────

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp(argv[0]);
        return 0;
    }

    try {
        Args args = parseArgs(argc, argv);

        if (args.demo) {
            runDemo();
        } else if (!args.osmFile.empty()) {
            runOSMRouting(args);
        } else {
            std::cerr << "[ERROR] Provide --demo or --osm <file.osm>\n";
            printHelp(argv[0]);
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "[FATAL] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
