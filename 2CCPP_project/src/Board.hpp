#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cctype>
using namespace std;

class Board {
    public:
        Board(int numPlayers);
        void display();
        void defineSize(int numPlayers);

    private:
        int size;
};