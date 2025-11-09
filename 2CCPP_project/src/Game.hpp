#pragma once
#include <vector>
#include "Player.hpp"
#include "Board.hpp"
#include "TileDeck.hpp"

namespace game
{

    void run(std::vector<Player> &players,
             Board &board,
             int rounds,
             const char *tileFilePath,
             unsigned seed);

}
