#pragma once
#include <string>
#include <vector>
#include <utility>
#include <ostream>

class Tile
{
public:
    Tile() = default;
    explicit Tile(const std::vector<std::pair<int, int>> &cells);

    const std::vector<std::pair<int, int>> &cells() const;
    int width() const;
    int height() const;

    std::vector<std::pair<int, int>> orientedCells(int rotDeg, bool flip) const;

    std::string toAscii(char filled = '#', char empty = ' ') const;

private:
    std::vector<std::pair<int, int>> m_cells;
};

std::ostream &operator<<(std::ostream &os, const Tile &t);
