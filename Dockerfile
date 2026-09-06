FROM ubuntu:22.04

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy source
COPY . .

# Configure and build
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON && \
    cmake --build build --config Release && \
    cd build && ctest --output-on-failure

# Default command: run demo
CMD ["./build/route_planner", "--demo"]
