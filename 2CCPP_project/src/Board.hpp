#pragma once
using namespace std;

class Board {
    public:
        Board(int numPlayers);
        void display();
        void defineSize(int numPlayers);

    private:
        int size;
};