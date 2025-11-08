#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cctype>
using namespace std;
#include "Board.hpp"

    void Board::display()
        {
            for (int i = 0; i < size; i++)
            {
                std::cout<<".";
            }
        }
        
    void Board::defineSize(int numPlayers)
        {
            if (numPlayers > 4)
            {
                size = 30;
            }
            else
            {
                size = 20;
            }
        }
        
    Board::Board(int numPlayers)
        {
            defineSize(numPlayers);
        }

    
