#pragma once
#include <vector>
#include <string>
#include "Tile.hpp"

class TileDeck
{
public:
    static TileDeck loadFromFile(const std::string &path, int numPlayers, unsigned seed);

    bool empty() const { return cursor_ >= tiles_.size(); }
    size_t size() const { return tiles_.size() - cursor_; }

    Tile draw();
    std::vector<Tile> peek(size_t k) const;
    Tile exchangeChoose(size_t idx);

    const std::vector<Tile> &all() const { return tiles_; }

private:
    std::vector<Tile> tiles_;
    size_t cursor_ = 0;

    static std::vector<Tile> parseFile(const std::string &path);
    static void shuffle(std::vector<Tile> &v, unsigned seed);
    static int quotaForPlayers(int numPlayers);
};
