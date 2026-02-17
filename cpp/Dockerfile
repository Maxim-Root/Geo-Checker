FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    cmake \
    qt6-base-dev \
    libprotobuf-dev \
    protobuf-compiler \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Build context: project root (geosite-program/)
# We receive cpp/ and proto/ as subdirs
WORKDIR /project
COPY cpp /project/cpp
COPY proto /project/proto

WORKDIR /project/cpp
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . -j$(nproc)
