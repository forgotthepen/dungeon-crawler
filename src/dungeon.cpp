#include "dungeon.h"
#include <random>
#include <optional>
#include <iostream>
#include <cmath>

Dungeon::Dungeon(int w, int h) 
    : width(w), height(h), currentSeed(0) { }

int Dungeon::getWidth() const {
    return width;
}

int Dungeon::getHeight() const {
    return height;
}

unsigned int Dungeon::getSeed() const {
    return currentSeed;
}

void Dungeon::resize(int w, int h) {
    width = w;
    height = h;
}

Vector2 Dungeon::getPlayerSpawn(int tileSize) const {
    return {
        (spawn.x * tileSize) + tileSize / 2.0f,
        (spawn.y * tileSize) + tileSize / 2.0f
    };
}

void Dungeon::generate(unsigned int seed) {
    currentSeed = seed;    
    while (0 == currentSeed) {
        currentSeed = std::random_device{}();
    }

    // reset map
    map.assign(height, std::vector<char>(width, 0));

    const int numRooms = 8;
    const float screenDistance = std::sqrt((float)width * width + (float)height * height);
    const int roomDistance = (int)(screenDistance / numRooms);
    
    // absolute parmeter
    std::uniform_int_distribution<int> distribRoomW(roomDistance / 2, roomDistance);
    std::uniform_int_distribution<int> distribRoomH(roomDistance / 2, roomDistance);
    // absolute position
    std::uniform_int_distribution<int> distribAbsPosX(1, width - roomDistance - 1);
    std::uniform_int_distribution<int> distribAbsPosY(1, height - roomDistance - 1);

    std::mt19937 rng(currentSeed);

    struct Room {
        int absPosX; // pos X on grid
        int absPosY; // pos Y on grid
        int width;   // width in pixels (points)
        int height;  // height in pixels (points)
    };

    std::optional<Room> lastRoom;

    for (int idx = 0; idx < numRooms; idx++) {
        Room newRoom;
        newRoom.absPosX = distribAbsPosX(rng);
        newRoom.absPosY = distribAbsPosY(rng);
        newRoom.width = distribRoomW(rng);
        newRoom.height = distribRoomH(rng);

        // carve room
        for (int yyy = newRoom.absPosY; yyy < newRoom.absPosY + newRoom.height && yyy < height - 1; yyy++) {
            for (int xxx = newRoom.absPosX; xxx < newRoom.absPosX + newRoom.width && xxx < width - 1; xxx++) {
                map[yyy][xxx] = WALKABLE;
            }
        }

        if (lastRoom) {
            // connect with corridor to previous room
            // start from the middle of the last room to the middle of the current room
            int corridorStartX = lastRoom.value().absPosX + lastRoom.value().width / 2;
            const int corridorEndX = newRoom.absPosX + newRoom.width / 2;
            const int incX = corridorStartX <= corridorEndX ? 1 : -1;

            int corridorStartY = lastRoom.value().absPosY + lastRoom.value().height / 2;
            const int corridorEndY = newRoom.absPosY + newRoom.height / 2;
            const int incY = corridorStartY <= corridorEndY ? 1 : -1;

            // carve corridor
            while (corridorStartX != corridorEndX) {
                map[corridorStartY][corridorStartX] = WALKABLE;
                corridorStartX += incX;
            }
            while (corridorStartY != corridorEndY) {
                map[corridorStartY][corridorStartX] = WALKABLE;
                corridorStartY += incY;
            }
        }

        lastRoom = newRoom;
    }

    // spawn = center of first room
    if (lastRoom) {
        spawn = {
            (float)(lastRoom.value().absPosX),
            (float)(lastRoom.value().absPosY)
        };
    }
}

void Dungeon::draw(const std::function<void(int posX, int posY)> &drawer) const {
    for (int yyy = 0; yyy < height; yyy++) {
        for (int xxx = 0; xxx < width; xxx++) {
            if (WALKABLE == map[yyy][xxx]) {
                drawer(xxx, yyy);
            }
        }
    }
}

bool Dungeon::isWalkable(int x, int y) const {
    if (x < 0 || x >= width) return false;
    if (y < 0 || y >= height) return false;

    return WALKABLE == map[y][x];
}
