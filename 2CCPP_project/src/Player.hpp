#pragma once
#include <string>

class Player
{
public:
    std::string name;
    std::string colorName;
    std::string colorCode;

    void assignColorToColorName();
    std::string getColoredName();
};
