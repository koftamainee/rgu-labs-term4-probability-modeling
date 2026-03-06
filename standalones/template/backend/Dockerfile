FROM debian:bookworm-slim AS builder

RUN apt-get update && apt-get install -y \
    cmake \
    ninja-build \
    g++ \
    git \
    libssl-dev \
    libboost-all-dev \
    libasio-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY CMakeLists.txt .
COPY src/ src/

RUN cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCROW_USE_BOOST=OFF \
    && cmake --build build --parallel

FROM debian:bookworm-slim AS runtime

RUN apt-get update && apt-get install -y \
    libssl3 \
    libboost-system1.74.0 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/backend .
COPY config.json .

EXPOSE 8080

CMD ["./backend", "config.json"]