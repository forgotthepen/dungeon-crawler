# Global for all stages
ARG USE_WAYLAND=OFF
ARG USE_SOUND=ON

FROM ubuntu:24.04 AS build

# Make the global ARG available inside this stage
ARG USE_WAYLAND
ARG USE_SOUND

# https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux

# Install build dependencies
RUN apt update && apt install -y \
  build-essential gcc g++ make pkg-config cmake cmake-data git \
  # X11
  libx11-dev libxrandr-dev libxi-dev \
  libgl1-mesa-dev  libglu1-mesa-dev \
  libxcursor-dev libxinerama-dev \
  # cleanup apt cache
  && rm -rf /var/lib/apt/lists/*
RUN if [[ "$USE_SOUND" = "ON" ]]; then \
  apt update && apt install -y \
  # sound
  libasound2-dev \
  # cleanup apt cache
  && rm -rf /var/lib/apt/lists/* \
; fi
RUN if [[ "$USE_WAYLAND" = "ON" ]]; then \
  apt update && apt install -y \
  # Wayland
  libwayland-dev libxkbcommon-dev wayland-protocols \
  # cleanup apt cache
  && rm -rf /var/lib/apt/lists/* \
; fi

# Set workdir
WORKDIR /app

# Copy only build files
COPY CMakeLists.txt ./
# Configure (this layer is cached unless CMakeLists changes)
RUN mkdir -p src/ && touch src/main.cpp && cmake -B build -S . \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_EXAMPLES=OFF \
  -DUSE_WAYLAND=$USE_WAYLAND

# Now copy sources (this will break cache only if src/ changes)
COPY src/ ./src/
COPY include/ ./include/
# Build (incremental if only .cpp files change)
RUN cmake --build build -j -v


# Runtime image (smaller, only contains the built game + runtime libs)
FROM ubuntu:24.04 AS runtime

# Make the global ARG available inside this stage
ARG USE_WAYLAND
ARG USE_SOUND

RUN apt update && apt install -y \
  # X11
  libx11-6 libxcursor1 libxrandr2 libxinerama1 \
  libgl1 libglu1-mesa libxi6 \
  # cleanup apt cache
  && rm -rf /var/lib/apt/lists/*
RUN if [[ "$USE_SOUND" = "ON" ]]; then \
  apt update && apt install -y \
  # sound
  libasound2t64 \
  # cleanup apt cache
  && rm -rf /var/lib/apt/lists/* \
; fi
RUN if [[ "$USE_WAYLAND" = "ON" ]]; then \
  apt update && apt install -y \
  # Wayland
  libwayland-client0 libxkbcommon0 \
  # cleanup apt cache
  && rm -rf /var/lib/apt/lists/* \
; fi

WORKDIR /app
COPY --from=build /app/build/DungeonCrawler .

CMD ["./DungeonCrawler"]
