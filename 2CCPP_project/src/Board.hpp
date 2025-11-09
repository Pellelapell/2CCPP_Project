#pragma once
#include <vector>
#include <string>
#include <random>
#include "Tile.hpp"
#include "Player.hpp"

enum class Bonus : unsigned char
{
    None = 0,
    Exchange,
    Stone,
    Robbery
};

struct Cell
{
    int owner = -1;
    bool stone = false;
    Bonus bonus = Bonus::None;
};

class Board
{
public:
    explicit Board(int numPlayers);

    int size() const { return N_; }

    std::string renderForHud(const std::vector<Player> &players) const;

    bool placeStart(int playerId, int r, int c);

    bool canPlace(const Tile &t, int top, int left, int playerId, bool firstTile) const;
    bool placeTile(const Tile &t, int top, int left, int playerId, bool firstTile,
                   std::vector<Bonus> &bonusesTaken);

    bool placeStone(int r, int c);
    bool removeStone(int r, int c);

    bool robOneCell(int targetPlayerId, int r, int c, int newOwnerId);

    int countOwned(int playerId) const;
    int largestSquareFor(int playerId) const;

private:
    int N_;
    std::vector<std::vector<Cell>> g_;
    std::mt19937 rng_;

    void distributeBonuses(int numPlayers);
    bool in(int r, int c) const { return (r >= 0 && r < N_ && c >= 0 && c < N_); }

    bool touchesOwnTerritory(const Tile &t, int top, int left, int playerId) const;
    bool touchesOtherSide(const Tile &t, int top, int left, int playerId) const;
    bool anyOverlapOrStone(const Tile &t, int top, int left) const;

    void applyBonusesAround(int r, int c, int owner, std::vector<Bonus> &out);
};
