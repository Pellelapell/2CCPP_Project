#pragma once
#include <string>

class Player
{
public:
    std::string name;
    std::string colorName;
    std::string colorCode;

    int getScore();
    void assignColorToColorName();
    std::string getColoredName();

private:
    int score;
};
