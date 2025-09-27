#pragma once
#include <vector>
#include <functional>
#include <raylib.h>

class Dungeon {
public:
    static constexpr int WALKABLE = 1;

    Dungeon(int width, int height);

    void generate(unsigned int seed = 0);
    void draw(const std::function<void(int posX, int posY)> &drawer) const;
    bool isWalkable(int x, int y) const;

    int getWidth() const;
    int getHeight() const;
    unsigned int getSeed() const;

    void resize(int w, int h);

    Vector2 getPlayerSpawn(int tileSize) const;

private:
    int width;
    int height;
    unsigned int currentSeed;
    std::vector<std::vector<int>> map{};
    Vector2 spawn{};
};
