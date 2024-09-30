# https://hub.docker.com/r/drogonframework/drogon
FROM ubuntu:24.04 AS build-dev
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ cmake git make gdb \
    openssl libssl-dev uuid-dev zlib1g-dev libpq-dev \
    libcxxopts-dev libjsoncpp-dev postgresql-all libsqlite3-dev

FROM build-dev as build
WORKDIR /app
COPY . .

RUN --mount=type=cache,target=/app/build,sharing=locked \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build  . -j$(nproc) && \
    mv output_bin/ ../

FROM ubuntu:24.04 AS runtime
RUN apt-get update && apt-get install -y --no-install-recommends \
    curl libssl3 libuuid1 zlib1g libjsoncpp25 libpq5 libsqlite3-0\
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=build /app/output_bin /app
COPY --from=build /app/config.json /app

ENV PORT=8000

HEALTHCHECK --interval=2m --timeout=30s --start-period=3m --retries=3 \
    CMD curl -f http://localhost:$PORT/healthcheck
CMD ./rest_server -p $PORT
