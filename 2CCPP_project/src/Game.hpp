#pragma once
#include <vector>
#include "Player.hpp"
#include "Board.hpp"
#include "TileDeck.hpp"

#include "Hud.hpp"

namespace game
{

    void run(std::vector<Player> &players,
             Board &board,
             int rounds,
             unsigned seed);
}
