#include "Hud.hpp"
#include <iostream>

static inline void clearScreen()
{
#ifdef _WIN32
    system("cls");
#endif
}

static void drawAsciiTileBlock(const Tile &t, std::ostream &out)
{
    const int H = t.height(), W = t.width();
    for (int r = 0; r < H; ++r)
    {
        for (int c = 0; c < W; ++c)
        {
            out << (t.containsRelative(r, c) ? "##" : "  ");
        }
        out << "\n";
    }
}

void Hud::printFrame(const Tile &current,
                     const std::vector<Tile> &next5,
                     int boardSize,
                     const std::vector<Player> &players,
                     int currentPlayerIndex,
                     int round)
{
    clearScreen();

    const Player &p = players[currentPlayerIndex];
    std::cout << p.colorCode << "Player " << (currentPlayerIndex + 1)
              << " (" << p.name << ") - Round " << round << "\033[0m\n";
    std::cout << "Current tile:\n";
    drawAsciiTileBlock(current, std::cout);

    std::cout << "\n\nNext tiles:\n";
    if (next5.empty())
    {
        std::cout << "(none)\n\n";
    }
    else
    {
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
                        std::cout << (t.containsRelative(r, c) ? "##" : "  ");
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

    std::cout << "Take tile (T)\n"
                 "Exchange tile (E)\n"
                 "Display queue (D)\n"
                 "> \n"
                 "Flip tile (F)\n"
                 "Rotate tile (R)\n"
                 "Place tile (P)\n"
                 "> ";
}
