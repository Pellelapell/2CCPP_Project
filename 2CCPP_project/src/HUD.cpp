#include "HUD.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
void HUD::drawAsciiTileBlock(const Tile &t, std::ostream &out)
{
    const int H = t.height();
    const int W = t.width();
    for (int r = 0; r < H; ++r)
    {
        for (int c = 0; c < W; ++c)
        {
            out << (t.containsRelative(r, c) ? "##" : "  ");
        }
        out << '\n';
    }
}

void HUD::printHeader(const std::vector<Player> &players, int currentPlayerIndex, int round)
{
    if (currentPlayerIndex >= 0 && currentPlayerIndex < static_cast<int>(players.size()))
    {
        const Player &p = players[currentPlayerIndex];
        std::cout << p.colorCode
                  << "Player " << (currentPlayerIndex + 1)
                  << " (" << p.name << ") - Round " << round
                  << "\033[0m\n";
    }
    else
    {
        std::cout << "Player ? - Round " << round << "\n";
    }
}

void HUD::printCurrentTile(const Tile &t)
{
    std::cout << "Current tile:\n";
    drawAsciiTileBlock(t, std::cout);
}

void HUD::drawColHeader(int cols)
{
    if (cols <= 0)
        return;
    std::cout << "Columns: ";
    for (int c = 0; c < cols; ++c)
        std::cout << c % 10;
    std::cout << "\n";
}

void HUD::printNextTiles(const std::vector<Tile> &next5)
{
    std::cout << "\nNext tiles:\n";
    if (next5.empty())
    {
        std::cout << "(none)\n\n";
        return;
    }

    int maxH = 0;
    const int n = static_cast<int>(std::min<std::size_t>(next5.size(), 5));
    for (int i = 0; i < n; ++i)
        maxH = std::max(maxH, next5[i].height());

    for (int r = 0; r < maxH; ++r)
    {
        for (int i = 0; i < n; ++i)
        {
            const Tile &t = next5[i];
            if (r < t.height())
            {
                for (int c = 0; c < t.width(); ++c)
                    std::cout << (t.containsRelative(r, c) ? "##" : "  ");
            }
            else
            {
                for (int c = 0; c < t.width(); ++c)
                    std::cout << "  ";
            }
            std::cout << "   ";
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

void HUD::printBoardEmpty(int boardSize)
{
    if (boardSize <= 0)
        return;
    std::cout << "Board size: " << boardSize << "x" << boardSize << "\n";
    drawColHeader(boardSize);
    std::cout << "(board rendering omitted)\n\n";
}

void HUD::printFooter(const Player &p)
{
    (void)p;
    std::cout
        << "Take tile (T)\n"
           "Exchange tile (E)\n"
           "Display queue (D)\n"
           "> \n"
           "Flip tile (F)\n"
           "Rotate tile (R)\n"
           "Place tile (P)\n"
           "> ";
}

void HUD::printFrame(const Tile &current,
                     const std::vector<Tile> &next5,
                     int boardSize,
                     const std::vector<Player> &players,
                     int currentPlayerIndex,
                     int round)
{

    printHeader(players, currentPlayerIndex, round);

    printCurrentTile(current);
    printNextTiles(next5);

    if (currentPlayerIndex >= 0 && currentPlayerIndex < static_cast<int>(players.size()))
        printFooter(players[currentPlayerIndex]);
    else
    {
        Player dummy;
        printFooter(dummy);
    }
}
