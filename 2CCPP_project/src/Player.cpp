#include "Player.hpp"
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <string>
using namespace std;

void Player::assignColorToColorName()
{
    static const std::unordered_map<std::string, std::string> ansiColorCode =
        {
            {"bleu", "\033[34m"},
            {"vert", "\033[32m"},
            {"jaune", "\033[33m"},
            {"orange", "\033[38;5;208m"},
            {"rouge", "\033[31m"},
            {"violet", "\033[35m"},
            {"rose", "\033[38;5;213m"},
            {"marron", "\033[38;5;94m"},
            {"blanc", "\033[37m"}};

    std::string key = colorName;
    std::transform(key.begin(), key.end(), key.begin(),
                   [](unsigned char c)
                   { return static_cast<char>(std::tolower(c)); });

    auto it = ansiColorCode.find(key);
    colorCode = (it != ansiColorCode.end() ? it->second : "\033[37m");
    colorName = (it != ansiColorCode.end() ? key : "blanc");
}

std::string Player::getColoredName()
{
    return colorCode + name + "(" + colorName + ")\033[0m";
}

int Player::getScore()
{
    return score;
}