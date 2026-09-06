# Graph-Based Route Planning Engine

A high-performance C++ shortest-path engine over real **OpenStreetMap** road networks,
featuring Dijkstra search, A\* heuristics, and exact multi-stop trip planning via
Held-Karp dynamic programming.

![Build](https://github.com/sathviknalla/route-planning-engine/actions/workflows/ci.yml/badge.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

---

## Features

✅ **Pathfinding Algorithms**
- **Dijkstra's Algorithm** — optimal shortest-path with min-heap priority queue
- **A\* Search** — informed search using admissible Haversine heuristic; faster than Dijkstra on geographic graphs
- **Held-Karp TSP Solver** — bitmask DP for exact multi-stop routing (up to 20 stops), O(n² · 2ⁿ)
- **2-opt Local Search** — post-processing tour improvement, O(n²) per iteration

✅ **Real OpenStreetMap Support**
- SAX-style, line-by-line XML parser — works on arbitrarily large `.osm` files
- Handles nodes, ways, `oneway`, `maxspeed`, `highway` tags
- Road-type default speed table (motorway → residential)
- Bidirectional edges with **travel-time weights** (seconds) and **distance** (metres)
- `parseOSMXMLFromString()` for in-memory/unit-test parsing

✅ **CLI Interface**
```bash
# Point-to-point with Dijkstra
./route_planner --osm map.osm --from 123456 --to 789012

# Point-to-point with A*
./route_planner --osm map.osm --from 123456 --to 789012 --algo astar

# Multi-stop TSP
./route_planner --osm map.osm --stops 111,222,333,444 --algo tsp

# Run built-in synthetic demo
./route_planner --demo

# Include benchmarks in output
./route_planner --osm map.osm --from 123456 --to 789012 --bench
```

✅ **Benchmarking**
- Wall-clock timing via `std::chrono::high_resolution_clock`
- Node expansion counting per query
- Peak RSS memory tracking (Linux)
- Formatted table output

---

## Project Structure

```
route-planning-engine/
├── include/
│   ├── graph/
│   │   └── Graph.h              # Graph data structure + Haversine helper
│   ├── algorithms/
│   │   ├── Dijkstra.h           # Dijkstra's algorithm
│   │   ├── AStar.h              # A* with Haversine heuristic
│   │   ├── HeldKarp.h           # TSP solver — Held-Karp bitmask DP
│   │   └── TwoOpt.h             # 2-opt tour improvement
│   ├── parser/
│   │   └── OSMParser.h          # OpenStreetMap XML parser
│   ├── benchmark/
│   │   └── Benchmark.h          # Benchmarking framework
│   └── utils/
│       └── Utils.h              # String helpers, formatters
├── src/
│   ├── graph/
│   │   └── Graph.cpp
│   ├── algorithms/
│   │   ├── Dijkstra.cpp
│   │   ├── AStar.cpp
│   │   ├── HeldKarp.cpp
│   │   └── TwoOpt.cpp
│   ├── parser/
│   │   └── OSMParser.cpp
│   ├── benchmark/
│   │   └── Benchmark.cpp
│   ├── utils/
│   │   └── Utils.cpp
│   └── main.cpp                 # CLI entry point
├── tests/
│   ├── GraphTest.cpp
│   ├── DijkstraTest.cpp
│   ├── AStarTest.cpp
│   ├── HeldKarpTest.cpp
│   ├── TwoOptTest.cpp
│   ├── IntegrationTest.cpp
│   └── RealWorldDataTest.cpp
├── examples/
│   └── sample.osm               # Minimal OSM XML for quick testing
├── CMakeLists.txt               # Build configuration (CMake 3.14+)
├── README.md
└── .gitignore
```

---

## Algorithm Complexity

| Algorithm | Time | Space | Best For |
|---|---|---|---|
| Dijkstra | O((V + E) log V) | O(V) | Single-source shortest path |
| A\* | O((V + E) log V) | O(V) | Single-pair (faster than Dijkstra on geo graphs) |
| Held-Karp | O(n² · 2ⁿ) | O(n · 2ⁿ) | Exact TSP, n ≤ 20 |
| 2-opt | O(n²) / iteration | O(n) | Local tour improvement |

*V = vertices, E = edges, n = number of stops*

---

## Building

### Prerequisites

| Tool | Version |
|---|---|
| C++ compiler | C++17 or later (g++, clang++, MSVC) |
| CMake | 3.14+ |
| Google Test | Fetched automatically via `FetchContent` |

No other external dependencies — the OSM parser is pure C++ standard library.

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/sathviknalla/route-planning-engine.git
cd route-planning-engine

# Configure and build (Release mode recommended)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

# Run the built-in demo
./build/route_planner --demo

# Run all unit tests
cd build && ctest --output-on-failure
```

---

## Usage Examples

### C++ API

```cpp
#include "graph/Graph.h"
#include "algorithms/Dijkstra.h"
#include "algorithms/AStar.h"
#include "parser/OSMParser.h"

int main() {
    Graph graph;

    // Load a real OSM file
    auto result = OSMParser::parseOSMXML("manhattan.osm", graph);
    // result.nodesParsed, result.edgesAdded, result.success

    // Point-to-point routing
    DijkstraResult dr = Dijkstra::findShortest(graph, 42439082, 61785543);
    if (dr.found) {
        // dr.distance (travel-time in seconds), dr.path (node IDs), dr.nodeExpansions
    }

    AStarResult ar = AStar::findShortest(graph, 42439082, 61785543);

    // Multi-stop TSP
    HeldKarpResult hr = HeldKarp::solve(graph, {42439082, 61785543, 105896613});
    // hr.totalDistance, hr.tour

    return 0;
}
```

### Getting OSM Data

Download a city extract from [Geofabrik](https://download.geofabrik.de/) or use the [Overpass API](https://overpass-api.de/):

```bash
# Small area bounding box (Overpass API)
curl "https://overpass-api.de/api/map?bbox=-74.02,40.70,-73.97,40.73" -o lower_manhattan.osm
./build/route_planner --osm lower_manhattan.osm --demo
```

---

## Running Tests

```bash
cd build
ctest --output-on-failure

# Or run individual test binaries
./tests/route_tests
```

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for coding standards and PR guidelines.

---

## License

MIT License — see [LICENSE](LICENSE).
