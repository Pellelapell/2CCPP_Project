#pragma once

#include <vector>
#include <string>
#include <ostream>

#include "Tile.hpp"
#include "Player.hpp"

class HUD
{
public:
    static void printFrame(const Tile &current,
                           const std::vector<Tile> &next5,
                           int boardSize,
                           const std::vector<Player> &players,
                           int currentPlayerIndex,
                           int round);

private:
    static void printHeader(const std::vector<Player> &players, int currentPlayerIndex, int round);
    static void printCurrentTile(const Tile &t);
    static void printNextTiles(const std::vector<Tile> &next5);
    static void printBoardEmpty(int boardSize);
    static void printFooter(const Player &p);

    static void drawColHeader(int cols);
    static void drawAsciiTileBlock(const Tile &t, std::ostream &out);
};
