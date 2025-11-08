#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cctype>
using namespace std;
#include "Board.hpp"

    void Board::display()
    {
        if (size == 20)
        {
            for (int i = 0; i < size; i++)
                {
                    for (int j = 0; j < size; j++)
                    {
                        std::cout << "....................";
                    }
                    std::cout << std::endl;
                }
        }
        else 
        {
            for (int i = 0; i < size; i++)
                {
                    for (int j = 0; j < size; j++)
                    {
                        std::cout << "..............................";
                    }
                    std::cout << std::endl;
                }
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

    
