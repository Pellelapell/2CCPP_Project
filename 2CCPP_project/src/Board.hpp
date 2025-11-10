#pragma once
#include <vector>
#include <ostream>
#include "Tile.hpp"

class Board
{
public:
    Board(int w, int h);

    int width() const { return W; }
    int height() const { return H; }
    bool inBounds(int x, int y) const;

    int at(int x, int y) const;
    void setCell(int x, int y, int v);

    bool canPlace(const Tile &t, int rotDeg, bool flip, int ox, int oy, int playerId) const;

    bool place(const Tile &t, int rotDeg, bool flip, int ox, int oy, int playerId);
    void render(std::ostream &os) const;

private:
    int W, H;
    std::vector<int> grid;

    int idx(int x, int y) const { return y * W + x; }
};
