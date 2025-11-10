#include "Board.hpp"
#include <cassert>

Board::Board(int w, int h) : W(w), H(h), grid(w * h, -1) {}

bool Board::inBounds(int x, int y) const
{
    return x >= 0 && x < W && y >= 0 && y < H;
}

int Board::at(int x, int y) const
{
    assert(inBounds(x, y));
    return grid[idx(x, y)];
}

void Board::setCell(int x, int y, int v)
{
    assert(inBounds(x, y));
    grid[idx(x, y)] = v;
}

bool Board::canPlace(const Tile &t, int rotDeg, bool flip, int ox, int oy, int playerId) const
{
    auto cells = t.orientedCells(rotDeg, flip);

    // 1) bornes + collisions
    for (auto [x, y] : cells)
    {
        int ax = ox + x;
        int ay = oy + y;
        if (!inBounds(ax, ay))
            return false;
        if (grid[idx(ax, ay)] != -1)
            return false; // case déjà occupée
    }

    // 2) doit toucher un voisin 4-connexe du même joueur (au moins une case)
    auto hasOwnNeighbor = [&](int ax, int ay) -> bool
    {
        static const int dx[4] = {1, -1, 0, 0};
        static const int dy[4] = {0, 0, 1, -1};
        for (int k = 0; k < 4; ++k)
        {
            int nx = ax + dx[k], ny = ay + dy[k];
            if (inBounds(nx, ny) && grid[idx(nx, ny)] == playerId)
                return true;
        }
        return false;
    };
    bool touchesOwn = false;
    for (auto [x, y] : cells)
    {
        if (hasOwnNeighbor(ox + x, oy + y))
        {
            touchesOwn = true;
            break;
        }
    }
    if (!touchesOwn)
        return false;

    // 3) ne doit pas toucher orthogonalement un autre joueur
    auto touchesOther = [&](int ax, int ay) -> bool
    {
        static const int dx[4] = {1, -1, 0, 0};
        static const int dy[4] = {0, 0, 1, -1};
        for (int k = 0; k < 4; ++k)
        {
            int nx = ax + dx[k], ny = ay + dy[k];
            if (inBounds(nx, ny))
            {
                int v = grid[idx(nx, ny)];
                if (v != -1 && v != playerId)
                    return true;
            }
        }
        return false;
    };
    for (auto [x, y] : cells)
    {
        if (touchesOther(ox + x, oy + y))
            return false;
    }

    return true;
}

bool Board::place(const Tile &t, int rotDeg, bool flip, int ox, int oy, int playerId)
{
    if (!canPlace(t, rotDeg, flip, ox, oy, playerId))
        return false;
    auto cells = t.orientedCells(rotDeg, flip);
    for (auto [x, y] : cells)
    {
        grid[idx(ox + x, oy + y)] = playerId;
    }
    return true;
}

void Board::render(std::ostream &os) const
{
    os << "\nBoard:\n";
    for (int y = 0; y < H; ++y)
    {
        for (int x = 0; x < W; ++x)
        {
            int v = grid[idx(x, y)];
            char ch = '.';
            if (v >= 0)
                ch = char('A' + (v % 26)); // ASCII simple
            else if (v == -2)
                ch = '#'; // pierre éventuelle
            os << ch << (x + 1 < W ? ' ' : '\n');
        }
    }
}
