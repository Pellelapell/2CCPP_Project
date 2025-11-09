#include "Hud.hpp"
#include <iostream>

static inline void clearScreen()
{
    std::cout << "\033[2J\033[H";
}

void Hud::printFrame(const Tile &current,
                     const std::vector<Tile> &next5,
                     int boardSize,
                     const std::vector<Player> &players,
                     int currentPlayerIndex,
                     int round)
{
    clearScreen();
    printHeader(players, currentPlayerIndex, round);
    printCurrentTile(current);
    printNextTiles(next5);
    printBoardEmpty(boardSize);
    printFooter(players[currentPlayerIndex]);
}

void Hud::printHeader(const std::vector<Player> &players, int idx, int round)
{
    const Player &p = players[idx];
    std::cout << "\033[1m" << p.colorCode
              << "Player " << (idx + 1) << " (" << p.name << ") - Round " << round
              << "\033[0m\n";
    std::cout << "Current tile:\n";
}

void Hud::drawAsciiTileBlock(const Tile &t, std::ostream &out)
{
    const int H = t.height(), W = t.width();
    for (int r = 0; r < H; ++r)
    {
        for (int c = 0; c < W; ++c)
        {
            bool filled = t.containsRelative(r, c);
            out << (filled ? "██" : "  ");
        }
        out << "\n";
    }
}

void Hud::printCurrentTile(const Tile &t)
{
    drawAsciiTileBlock(t, std::cout);
    std::cout << "\n\nNext tiles:\n";
}

void Hud::printNextTiles(const std::vector<Tile> &next5)
{
    if (next5.empty())
    {
        std::cout << "(none)\n\n";
        return;
    }

    int maxH = 0;
    for (int i = 0; i < (int)next5.size() && i < 5; ++i)
        maxH = std::max(maxH, next5[i].height());

    for (int r = 0; r < maxH; ++r)
    {
        for (int i = 0; i < (int)next5.size() && i < 5; ++i)
        {
            const Tile &t = next5[i];
            if (r < t.height())
            {
                for (int c = 0; c < t.width(); ++c)
                {
                    std::cout << (t.containsRelative(r, c) ? "██" : "  ");
                }
            }
            else
            {
                for (int c = 0; c < t.width(); ++c)
                    std::cout << "  ";
            }
            std::cout << "   ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void Hud::drawColHeader(int cols)
{
    std::cout << " ";
    for (int c = 0; c < cols; ++c)
    {
        char ch = (c < 26) ? char('A' + c) : '.';
        std::cout << ch;
    }
    std::cout << "\n";
}

void Hud::printBoardEmpty(int boardSize)
{
    drawColHeader(boardSize);
    for (int r = 0; r < boardSize; ++r)
    {
        char row = (r < 26) ? char('A' + r) : '.';
        std::cout << row;
        for (int c = 0; c < boardSize; ++c)
            std::cout << "\033[2m.\033[0m";
        std::cout << "\n";
    }
    std::cout << "\n";
}

void Hud::printFooter(const Player & /*p*/)
{
    std::cout << "Take tile (T)\n"
                 "Exchange tile (E)\n"
                 "Display queue (D)\n"
                 "> \n"
                 "Flip tile (F)\n"
                 "Rotate tile (R)\n"
                 "Place tile (P)\n"
                 "> ";
}
