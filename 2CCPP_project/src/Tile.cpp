#include "Tile.hpp"
#include <algorithm>
#include <sstream>
#include <utility>

Tile::Tile(int id_, std::string name_, std::vector<Cell> cells_)
    : id(id_), name(std::move(name_)), cells(std::move(cells_)) { normalize(); }

Tile Tile::from01Mask(int id, const std::string &name,
                      const std::vector<std::string> &mask)
{
    std::vector<Cell> cells;
    for (int r = 0; r < (int)mask.size(); ++r)
    {
        const auto &row = mask[r];
        for (int c = 0; c < (int)row.size(); ++c)
            if (row[c] == '1')
                cells.push_back({r, c});
    }
    return Tile{id, name, std::move(cells)};
}

int Tile::maxRow() const
{
    int m = 0;
    for (auto &p : cells)
        m = std::max(m, p.r);
    return m;
}
int Tile::maxCol() const
{
    int m = 0;
    for (auto &p : cells)
        m = std::max(m, p.c);
    return m;
}
int Tile::width() const { return cells.empty() ? 0 : maxCol() + 1; }
int Tile::height() const { return cells.empty() ? 0 : maxRow() + 1; }

void Tile::normalize()
{
    if (cells.empty())
        return;
    int minr = cells[0].r, minc = cells[0].c;
    for (auto &p : cells)
    {
        minr = std::min(minr, p.r);
        minc = std::min(minc, p.c);
    }
    for (auto &p : cells)
    {
        p.r -= minr;
        p.c -= minc;
    }
}

void Tile::rotateCW()
{
    int H = height();
    for (auto &p : cells)
    {
        int nr = p.c, nc = H - 1 - p.r;
        p.r = nr;
        p.c = nc;
    }
    normalize();
}
void Tile::rotateCCW()
{
    int W = width();
    for (auto &p : cells)
    {
        int nr = W - 1 - p.c, nc = p.r;
        p.r = nr;
        p.c = nc;
    }
    normalize();
}
void Tile::flipH()
{
    int W = width();
    for (auto &p : cells)
        p.c = W - 1 - p.c;
    normalize();
}
void Tile::flipV()
{
    int H = height();
    for (auto &p : cells)
        p.r = H - 1 - p.r;
    normalize();
}

Tile Tile::rotatedCW() const
{
    Tile t = *this;
    t.rotateCW();
    return t;
}
Tile Tile::rotatedCCW() const
{
    Tile t = *this;
    t.rotateCCW();
    return t;
}
Tile Tile::flippedH() const
{
    Tile t = *this;
    t.flipH();
    return t;
}
Tile Tile::flippedV() const
{
    Tile t = *this;
    t.flipV();
    return t;
}

std::vector<Cell> Tile::translated(int top, int left) const
{
    std::vector<Cell> out;
    out.reserve(cells.size());
    for (auto &p : cells)
        out.push_back({p.r + top, p.c + left});
    return out;
}
bool Tile::containsRelative(int r, int c) const
{
    for (auto &p : cells)
        if (p.r == r && p.c == c)
            return true;
    return false;
}
std::string Tile::toAscii(char filled, char empty) const
{
    std::ostringstream oss;
    int H = height(), W = width();
    for (int r = 0; r < H; ++r)
    {
        for (int c = 0; c < W; ++c)
            oss << (containsRelative(r, c) ? filled : empty);
        if (r + 1 < H)
            oss << '\n';
    }
    return oss.str();
}
std::ostream &operator<<(std::ostream &os, const Tile &t)
{
    os << t.toAscii();
    return os;
}
