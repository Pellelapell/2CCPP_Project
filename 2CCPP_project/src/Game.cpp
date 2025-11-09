#include "Game.hpp"
#include <iostream>
#include <cctype>

namespace game
{

    static int askRow()
    {
        int r;
        std::cin >> r;
        return r - 1;
    }
    static int askCol()
    {
        char ch;
        std::cin >> ch;
        return int(std::toupper((unsigned char)ch) - 'A');
    }

    void run(std::vector<Player> &players,
             Board &board,
             int rounds,
             const char *tileFilePath,
             unsigned seed)
    {
        const int N = static_cast<int>(players.size());

        TileDeck deck = TileDeck::loadFromFile(tileFilePath, N, seed);

        for (int round = 1; round <= rounds; ++round)
        {
            std::cout << "\n================= MANCHE " << round << " / " << rounds << " =================\n";

            for (int turn = 0; turn < N; ++turn)
            {
                Player &P = players[turn];

                auto next5 = deck.peek(5);
                std::cout << "\nTour de " << P.getColoredName() << "\n";
                if (!next5.empty())
                {
                    std::cout << "Prochaines tuiles (0..4) :\n";
                    for (int i = 0; i < (int)next5.size(); ++i)
                    {
                        std::cout << "[" << i << "]\n"
                                  << next5[i] << "\n\n";
                    }
                }

                char act = 'T';
                if (!next5.empty())
                {
                    std::cout << "(T)irer ou (E)changer 0..4 ? ";
                    std::cin >> act;
                }

                Tile current;
                if (act == 'E' || act == 'e')
                {
                    int idx = 0;
                    std::cout << "Indice de 0 a 4 : ";
                    std::cin >> idx;
                    if (idx < 0)
                        idx = 0;
                    if (idx > 4)
                        idx = 4;
                    current = deck.exchangeChoose((size_t)idx);
                }
                else
                {
                    current = deck.draw();
                }

                std::cout << "Tuile tiree (id=" << current.id << ") :\n"
                          << current << "\n";

                while (true)
                {
                    std::cout << "(F)lip, (R)otate, (P)ose, (S)kip ? ";
                    char cmd;
                    std::cin >> cmd;

                    if (cmd == 'F' || cmd == 'f')
                    {
                        current.flipH();
                        std::cout << "\nTuile :\n"
                                  << current << "\n";
                    }
                    else if (cmd == 'R' || cmd == 'r')
                    {
                        current.rotateCW();
                        std::cout << "\nTuile :\n"
                                  << current << "\n";
                    }
                    else if (cmd == 'P' || cmd == 'p')
                    {
                        std::cout << "OK, tu as pose la tuile (pas de rendu cellulaire pour l'instant).\n";
                        break;
                    }
                    else
                    {
                        std::cout << "Tuile defaussee.\n";
                        break;
                    }
                }

                std::cout << "\nPlateau :\n";
                board.display();
            }
        }

        std::cout << "\n== Fin de partie apres " << rounds << " manches ==\n";
        std::cout << "(Tu pourras brancher le vrai comptage des scores quand ton Board aura une grille.)\n";
    }

}
