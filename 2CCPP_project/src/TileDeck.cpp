#include "TileDeck.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <cctype>
#include <cmath>
#include "TileData.hpp";

static std::vector<std::string> cleanData(const std::vector<std::string> &in)
{
    if (in.empty())
        return in;
    int H = (int)in.size();
    int W = 0;
    for (auto &s : in)
        W = std::max(W, (int)s.size());
    int lastOne = -1;
    for (int c = 0; c < W; ++c)
    {
        for (int r = 0; r < H; ++r)
        {
            if (c < (int)in[r].size() && in[r][c] == '1')
            {
                lastOne = c;
                break;
            }
        }
    }
    if (lastOne < 0)
        return {};
    std::vector<std::string> out = in;
    for (auto &s : out)
        if ((int)s.size() > lastOne + 1)
            s.erase(s.begin() + lastOne + 1, s.end());
    return out;
}

static std::vector<Tile> parse01Tiles(std::istream &is)
{
    std::vector<Tile> tiles;
    std::string line;
    std::vector<std::string> block;

    auto pushBlock = [&]()
    {
        if (block.empty())
            return;
        std::vector<std::string> mask;
        for (auto row : block)
        {
            std::string cleaned;
            for (char ch : row)
                if (ch == '0' || ch == '1')
                    cleaned.push_back(ch);
            mask.push_back(cleaned);
        }
        mask = cleanData(mask);
        if (!mask.empty())
        {
            int id = (int)tiles.size();
            tiles.push_back(Tile::from01Mask(id, "T" + std::to_string(id), mask));
        }
        block.clear();
    };

    while (std::getline(is, line))
    {
        bool onlySpaces = true;
        for (char ch : line)
            if (!std::isspace((unsigned char)ch))
            {
                onlySpaces = false;
                break;
            }
        if (onlySpaces)
            pushBlock();
        else
            block.push_back(line);
    }
    pushBlock();
    return tiles;
}

std::vector<Tile> TileDeck::parseFile(const std::string &path)
{
    std::ifstream f(path);
    if (!f)
        return {};
    return parse01Tiles(f);
}
void TileDeck::shuffle(std::vector<Tile> &v, unsigned seed)
{
    std::mt19937 rng(seed);
    std::shuffle(v.begin(), v.end(), rng);
}
int TileDeck::quotaForPlayers(int n)
{
    int q = (int)std::lround(10.67 * n);
    return std::max(1, q);
}

TileDeck TileDeck::loadFromFile(const std::string &path, int numPlayers, unsigned seed)
{
    std::vector<Tile> all = parseFile(path);
    shuffle(all, seed);
    int q = std::min((int)all.size(), quotaForPlayers(numPlayers));
    TileDeck d;
    d.tiles_.assign(all.begin(), all.begin() + q);
    return d;
}

Tile TileDeck::draw()
{
    if (empty())
        return Tile{};
    return tiles_[cursor_++];
}
std::vector<Tile> TileDeck::peek(size_t k) const
{
    std::vector<Tile> out;
    for (size_t i = 0; i < k && cursor_ + i < tiles_.size(); ++i)
        out.push_back(tiles_[cursor_ + i]);
    return out;
}
Tile TileDeck::exchangeChoose(size_t idx)
{
    if (idx >= 5 || cursor_ + idx >= tiles_.size())
        return draw();
    Tile chosen = tiles_[cursor_ + idx];

    std::vector<Tile> next;
    for (size_t i = cursor_ + idx + 1; i < tiles_.size(); ++i)
        next.push_back(tiles_[i]);
    for (size_t i = cursor_; i < cursor_ + idx; ++i)
        next.push_back(tiles_[i]);

    std::vector<Tile> consumed(tiles_.begin(), tiles_.begin() + cursor_);
    consumed.push_back(chosen);
    consumed.insert(consumed.end(), next.begin(), next.end());
    tiles_.swap(consumed);

    ++cursor_;
    return chosen;
}
