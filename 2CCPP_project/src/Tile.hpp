#pragma once
#include <vector>
#include <string>
#include <ostream>

struct Cell
{
    int r = 0;
    int c = 0;
};

class Tile
{
public:
    int id = -1;
    std::string name;
    std::vector<Cell> cells;

    Tile() = default;
    Tile(int id_, std::string name_, std::vector<Cell> cells_);

    static Tile from01Mask(int id, const std::string &name,
                           const std::vector<std::string> &mask);

    int width() const;
    int height() const;
    void normalize();
    void rotateCW();
    void rotateCCW();
    void flipH();
    void flipV();

    Tile rotatedCW() const;
    Tile rotatedCCW() const;
    Tile flippedH() const;
    Tile flippedV() const;

    std::vector<Cell> translated(int top, int left) const;
    bool containsRelative(int r, int c) const;
    std::string toAscii(char filled = '#', char empty = ' ') const;

private:
    int maxRow() const;
    int maxCol() const;
};

std::ostream &operator<<(std::ostream &os, const Tile &t);
