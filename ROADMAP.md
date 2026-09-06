# 🗺️ Project Roadmap

## Phase 1: Core Engine & Streaming Parser ✅
- [x] High-performance adjacency-list `Graph` with Haversine distance.
- [x] Dijkstra shortest path with min-heap priority queue.
- [x] A* heuristic search using geographic distance.
- [x] Held-Karp exact dynamic programming TSP solver.
- [x] 2-opt local search post-processor.
- [x] Streaming SAX-style OpenStreetMap XML parser.
- [x] CLI interface with synthetic demo mode and benchmarking.

## Phase 2: Testing, Build System & CI/CD ✅
- [x] Modular CMake setup with `route_planning_lib` and GoogleTest via `FetchContent`.
- [x] Unit test suite covering all graph operations, routing algorithms, parser, and 2-opt swaps.
- [x] Sample OpenStreetMap XML dataset (`data/sample_map.osm`).
- [x] Multi-platform GitHub Actions CI matrix (Linux, Windows, macOS).

## Phase 3: Performance & Advanced Routing 🚀
- [ ] **Contraction Hierarchies (CH)** for sub-millisecond continental route queries.
- [ ] **Customizable Cost Profiles** (car, bicycle, pedestrian, truck with height/weight limits).
- [ ] **Turn Penalties & Traffic Ingestion** for realistic city routing.
- [ ] **OSM PBF Parser Integration** via `libosmium` or zero-copy protobuf parsing.
- [ ] **Christofides 3/2-Approximation Algorithm** for large metric TSP problems ($N > 20$).

## Phase 4: Web Services & Visualization 🌐
- [ ] Header-only REST API / Crow HTTP server for route JSON responses.
- [ ] GeoJSON export for path geometry.
- [ ] Leaflet.js interactive map frontend for visualization.
