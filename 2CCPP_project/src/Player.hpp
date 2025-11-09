#pragma once
#include <string>

struct Player
{
    int id = -1;
    std::string name;
    std::string colorName;
    std::string colorCode;
    int coupons = 1;
    int cellsOwned = 0;
    int startR = -1;
    int startC = -1;

    void assignColorToColorName();
    std::string getColoredName();
    int getScore();
};
