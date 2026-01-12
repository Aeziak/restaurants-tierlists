# Build stage
FROM gcc:13 AS builder

# Install dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    git \
    libssl-dev \
    libpq-dev \
    zlib1g-dev \
    libbrotli-dev \
    uuid-dev \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Install Drogon dependencies via vcpkg
WORKDIR /opt
RUN git clone https://github.com/Microsoft/vcpkg.git \
    && cd vcpkg \
    && ./bootstrap-vcpkg.sh \
    && ./vcpkg install drogon jwt-cpp redis-plus-plus

# Set vcpkg toolchain
ENV CMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake

# Copy backend source
WORKDIR /app
COPY . .

RUN git submodule update --init --recursive || true

# Build backend
RUN cmake -B build -S . -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build/ --config Release

# Runtime stage
FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libpq5 \
    libssl3 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Copy built executable
COPY --from=builder /app/build/restaurant_tier_list_backend /usr/local/bin/

EXPOSE 8080

CMD ["restaurant_tier_list_backend"]
