## Build

### Docker build

#### X11
```shell
docker build -t dungeon-crawler:latest-x11 .
```

#### Wayland
```shell
docker build -t dungeon-crawler:latest-wayland --build-arg USE_WAYLAND=ON .
```

### Native OS build

#### Dependencies
```shell
sudo apt update
sudo apt install -y \
  cmake gcc g++ git make \
  # sound
  libasound2-dev \
  # X11
  libx11-dev libxrandr-dev libxi-dev \
  libgl1-mesa-dev  libglu1-mesa-dev \
  libxcursor-dev libxinerama-dev \
  # Wayland
  libwayland-dev libxkbcommon-dev wayland-protocols
```

#### Generate build files

##### X11
```shell
cmake -B build -S . \
  -DUSE_WAYLAND=OFF
```

##### Wayland
```shell
cmake -B build -S . \
  -DUSE_WAYLAND=ON
```

#### Build project
```shell
cmake --build build -j -v
```

---

## Run

### Docker

#### X11
```shell
# allow Docker to use your X server
xhost +local:docker
# solve selinux problem on Fedora: https://unix.stackexchange.com/a/387744
docker run --rm -it --security-opt label=type:container_runtime_t \
  -e "DISPLAY=$DISPLAY" \
  -v "/tmp/.X11-unix:/tmp/.X11-unix" \
  dungeon-crawler:latest-x11
```

#### Wayland
```shell
docker run --rm -it --security-opt label=type:container_runtime_t \
  -e "SDL_VIDEODRIVER=wayland" \
  -e "WAYLAND_DISPLAY=$WAYLAND_DISPLAY" \
  -v "/run/user/$(id -u)/$WAYLAND_DISPLAY:/run/user/$(id -u)/$WAYLAND_DISPLAY" \
  dungeon-crawler:latest-wayland
```

### Native OS

```shell
chmod +x ./build/DungeonCrawler
./build/DungeonCrawler
```
