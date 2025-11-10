#include "Tile.hpp"
#include <algorithm>
#include <sstream>
#include <cassert>

static std::vector<std::pair<int, int>>
normalize(std::vector<std::pair<int, int>> pts)
{
    if (pts.empty())
        return pts;
    int minx = pts[0].first, miny = pts[0].second;
    for (auto &p : pts)
    {
        minx = std::min(minx, p.first);
        miny = std::min(miny, p.second);
    }
    for (auto &p : pts)
    {
        p.first -= minx;
        p.second -= miny;
    }
    return pts;
}

Tile::Tile(const std::vector<std::pair<int, int>> &cells)
    : m_cells(normalize(cells)) {}

const std::vector<std::pair<int, int>> &Tile::cells() const { return m_cells; }

int Tile::width() const
{
    int maxx = 0;
    for (auto &p : m_cells)
        maxx = std::max(maxx, p.first);
    return m_cells.empty() ? 0 : maxx + 1;
}

int Tile::height() const
{
    int maxy = 0;
    for (auto &p : m_cells)
        maxy = std::max(maxy, p.second);
    return m_cells.empty() ? 0 : maxy + 1;
}

std::vector<std::pair<int, int>>
Tile::orientedCells(int rotDeg, bool flip) const
{
    assert(rotDeg == 0 || rotDeg == 90 || rotDeg == 180 || rotDeg == 270);
    std::vector<std::pair<int, int>> out;
    out.reserve(m_cells.size());
    for (auto [x, y] : m_cells)
    {
        int xr = x, yr = y;
        switch (rotDeg)
        {
        case 0:
            xr = x;
            yr = y;
            break;
        case 90:
            xr = -y;
            yr = x;
            break;
        case 180:
            xr = -x;
            yr = -y;
            break;
        case 270:
            xr = y;
            yr = -x;
            break;
        }
        if (flip)
            xr = -xr;
        out.emplace_back(xr, yr);
    }
    return normalize(out);
}

std::string Tile::toAscii(char filled, char empty) const
{
    int w = width(), h = height();
    if (w <= 0 || h <= 0)
        return {};
    std::vector<std::string> g(h, std::string(w, empty));
    for (auto [x, y] : m_cells)
    {
        if (y >= 0 && y < h && x >= 0 && x < w)
            g[y][x] = filled;
    }
    std::ostringstream oss;
    for (int y = 0; y < h; ++y)
    {
        oss << g[y];
        if (y + 1 < h)
            oss << '\n';
    }
    return oss.str();
}

std::ostream &operator<<(std::ostream &os, const Tile &t)
{
    os << t.toAscii();
    return os;
}
