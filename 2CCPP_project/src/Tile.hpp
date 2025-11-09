#pragma once
#include <vector>
#include <string>
#include <ostream>

struct Point
{
    int r;
    int c;
};

class Tile
{
public:
    int id = -1;
    std::string name;
    std::vector<Point> cells; // <-- Point, pas Cell

    Tile() = default;
    Tile(int id_, std::string name_, std::vector<Point> cells_);

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

    std::vector<Point> translated(int top, int left) const; // <-- Point
    bool containsRelative(int r, int c) const;
    std::string toAscii(char filled = '#', char empty = ' ') const;

private:
    int maxRow() const;
    int maxCol() const;
};

std::ostream &operator<<(std::ostream &os, const Tile &t);
