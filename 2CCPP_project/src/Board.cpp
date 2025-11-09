#include "Board.hpp"
#include <sstream>
#include <algorithm>
#include <ctime>

static int ceilMul(double a, int n)
{
    return (int)std::ceil(a * n);
}

Board::Board(int numPlayers)
    : N_((numPlayers > 4) ? 30 : 20),
      g_(N_, std::vector<Cell>(N_))
{
    rng_.seed((unsigned)time(nullptr));
    distributeBonuses(numPlayers);
}

std::string Board::renderForHud(const std::vector<Player> &players) const
{
    std::ostringstream oss;
    oss << " ";
    for (int c = 0; c < N_; ++c)
    {
        char ch = (c < 26) ? char('A' + c) : '.';
        oss << ch;
    }
    oss << "\n";
    for (int r = 0; r < N_; ++r)
    {
        char row = (r < 26) ? char('A' + r) : '.';
        oss << row;
        for (int c = 0; c < N_; ++c)
        {
            const Cell &cell = g_[r][c];
            char dot = '.';
            if (cell.stone)
                dot = 'S';
            else if (cell.owner >= 0)
            {
                char ini = (players[cell.owner].name.empty() ? '0' : (char)std::tolower(players[cell.owner].name[0]));
                dot = ini;
            }
            else if (cell.bonus != Bonus::None)
            {
                dot = (cell.bonus == Bonus::Exchange ? 'E' : (cell.bonus == Bonus::Stone ? 'R' : 'B')); // E, R(rock), B(robbery)
            }
            oss << dot;
        }
        oss << "\n";
    }
    return oss.str();
}

void Board::distributeBonuses(int numPlayers)
{
    int cntExchange = (int)std::ceil(1.5 * numPlayers);
    int cntStone = (int)std::ceil(0.5 * numPlayers);
    int cntRobbery = numPlayers;

    auto place = [&](Bonus b, int count)
    {
        std::uniform_int_distribution<int> dist(1, N_ - 2);
        while (count > 0)
        {
            int r = dist(rng_), c = dist(rng_);
            if (g_[r][c].bonus != Bonus::None || g_[r][c].stone || g_[r][c].owner != -1)
                continue;
            bool ok = true;
            for (int d = 0; d < 4; ++d)
            {
                int nr = r + (int[]){-1, 1, 0, 0}[d];
                int nc = c + (int[]){0, 0, -1, 1}[d];
                if (in(nr, nc) && g_[nr][nc].bonus != Bonus::None)
                {
                    ok = false;
                    break;
                }
            }
            if (!ok)
                continue;
            g_[r][c].bonus = b;
            --count;
        }
    };
    place(Bonus::Exchange, cntExchange);
    place(Bonus::Stone, cntStone);
    place(Bonus::Robbery, cntRobbery);
}

bool Board::placeStart(int playerId, int r, int c)
{
    if (!in(r, c) || g_[r][c].owner != -1 || g_[r][c].stone)
        return false;
    g_[r][c].owner = playerId;
    return true;
}

bool Board::anyOverlapOrStone(const Tile &t, int top, int left) const
{
    for (auto p : t.translated(top, left))
    {
        if (!in(p.r, p.c))
            return true;
        if (g_[p.r][p.c].owner != -1)
            return true;
        if (g_[p.r][p.c].stone)
            return true;
    }
    return false;
}

bool Board::touchesOwnTerritory(const Tile &t, int top, int left, int playerId) const
{
    for (auto p : t.translated(top, left))
    {
        const int dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
        for (int k = 0; k < 4; ++k)
        {
            int nr = p.r + dr[k], nc = p.c + dc[k];
            if (in(nr, nc) && g_[nr][nc].owner == playerId)
                return true;
        }
    }
    return false;
}

bool Board::touchesOtherSide(const Tile &t, int top, int left, int playerId) const
{
    for (auto p : t.translated(top, left))
    {
        const int dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
        for (int k = 0; k < 4; ++k)
        {
            int nr = p.r + dr[k], nc = p.c + dc[k];
            if (in(nr, nc) && g_[nr][nc].owner != -1 && g_[nr][nc].owner != playerId)
                return true;
        }
    }
    return false;
}

bool Board::canPlace(const Tile &t, int top, int left, int playerId, bool firstTile) const
{
    if (anyOverlapOrStone(t, top, left))
        return false;
    if (!firstTile && !touchesOwnTerritory(t, top, left, playerId))
        return false;
    if (touchesOtherSide(t, top, left, playerId))
        return false;
    return true;
}

void Board::applyBonusesAround(int r, int c, int owner, std::vector<Bonus> &out)
{
    const int dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
    for (int k = 0; k < 4; ++k)
    {
        int nr = r + dr[k], nc = c + dc[k];
        if (in(nr, nc) && g_[nr][nc].bonus != Bonus::None)
        {
            out.push_back(g_[nr][nc].bonus);
            g_[nr][nc].owner = owner;
            g_[nr][nc].bonus = Bonus::None;
        }
    }
}

bool Board::placeTile(const Tile &t, int top, int left, int playerId, bool firstTile,
                      std::vector<Bonus> &bonusesTaken)
{
    if (!canPlace(t, top, left, playerId, firstTile))
        return false;
    for (auto p : t.translated(top, left))
    {
        g_[p.r][p.c].owner = playerId;
        applyBonusesAround(p.r, p.c, playerId, bonusesTaken);
    }
    return true;
}

bool Board::placeStone(int r, int c)
{
    if (!in(r, c) || g_[r][c].owner != -1 || g_[r][c].stone)
        return false;
    g_[r][c].stone = true;
    return true;
}

bool Board::removeStone(int r, int c)
{
    if (!in(r, c) || !g_[r][c].stone)
        return false;
    g_[r][c].stone = false;
    return true;
}

bool Board::robOneCell(int targetPlayerId, int r, int c, int newOwnerId)
{
    if (!in(r, c))
        return false;
    if (g_[r][c].owner != targetPlayerId)
        return false;
    g_[r][c].owner = newOwnerId;
    return true;
}

int Board::countOwned(int playerId) const
{
    int cnt = 0;
    for (int r = 0; r < N_; ++r)
        for (int c = 0; c < N_; ++c)
            if (g_[r][c].owner == playerId)
                ++cnt;
    return cnt;
}

int Board::largestSquareFor(int playerId) const
{
    std::vector<std::vector<int>> dp(N_, std::vector<int>(N_, 0));
    int best = 0;
    for (int r = 0; r < N_; ++r)
    {
        for (int c = 0; c < N_; ++c)
        {
            if (g_[r][c].owner == playerId)
            {
                if (r == 0 || c == 0)
                    dp[r][c] = 1;
                else
                    dp[r][c] = 1 + std::min({dp[r - 1][c], dp[r][c - 1], dp[r - 1][c - 1]});
                best = std::max(best, dp[r][c]);
            }
        }
    }
    return best;
}
