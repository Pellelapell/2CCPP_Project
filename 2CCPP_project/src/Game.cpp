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

    static inline int boardSizeForPlayers(size_t n)
    {
        return (n > 4 ? 30 : 20);
    }

    void run(std::vector<Player> &players,
             Board &board,
             int rounds,
             unsigned seed)
    {
        const int N = static_cast<int>(players.size());
        const int boardSize = boardSizeForPlayers(players.size());

        TileDeck deck = TileDeck::loadBuiltin(N, seed);

        for (int round = 1; round <= rounds; ++round)
        {
            std::cout << "\n================= MANCHE " << round << " / " << rounds << " =================\n";

            for (int turn = 0; turn < N; ++turn)
            {
                Player &P = players[turn];

                auto next5 = deck.peek(5);

                Tile current;
                Hud::printFrame(current, next5, boardSize, players, turn, round);

                char act = 'T';
                if (!next5.empty())
                {
                    std::cout << "(T)irer ou (E)changer 0..4 ? ";
                    std::cin >> act;
                }

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

                next5 = deck.peek(5);
                Hud::printFrame(current, next5, boardSize, players, turn, round);

                bool placed = false;
                while (true)
                {
                    std::cout << "(F)lip, (R)otate, (P)ose, (S)kip ? ";
                    char cmd;
                    std::cin >> cmd;

                    if (cmd == 'F' || cmd == 'f')
                    {
                        current.flipH();
                        Hud::printFrame(current, next5, boardSize, players, turn, round);
                    }
                    else if (cmd == 'R' || cmd == 'r')
                    {
                        current.rotateCW();
                        Hud::printFrame(current, next5, boardSize, players, turn, round);
                    }
                    else if (cmd == 'P' || cmd == 'p')
                    {
                        // Avec ton Board actuel (rendu en points uniquement),
                        // on valide la pose “logiquement” et on passe au suivant.
                        // Quand tu auras un Board à grille, tu pourras demander ligne/colonne ici
                        // et appeler board.placeTile(...)
                        std::cout << "Tuile posee (rendu plateau simplifie pour le moment).\n";
                        placed = true;
                        break;
                    }
                    else
                    { // Skip
                        std::cout << "Tuile defaussee.\n";
                        break;
                    }
                }

                // Affiche le plateau (points) à chaque fin de tour
                std::cout << "\nPlateau :\n";
                board.display();
            }
        }

        std::cout << "\n== Fin de partie apres " << rounds << " manches ==\n";
    }

    // Rétro-compatibilité : on ignore le chemin et on appelle la version intégrée
    void run(std::vector<Player> &players,
             Board &board,
             int rounds,
             const char * /*filePathIgnored*/,
             unsigned seed)
    {
        run(players, board, rounds, seed);
    }

} // namespace game
