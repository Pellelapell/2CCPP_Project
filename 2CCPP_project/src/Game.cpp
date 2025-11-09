#include "Game.hpp"
#include <iostream>
#include <random>
#include <algorithm>
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
    static inline int boardSizeForPlayers(size_t n) { return (n > 4 ? 30 : 20); }

    static void waitEnter()
    {
        std::cout << "\n(Entrer pour continuer)";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (std::cin.peek() == '\n')
            std::cin.get();
    }

    void run(std::vector<Player> &players, Board &board, int rounds, unsigned seed)
    {
        const int N = (int)players.size();
        std::mt19937 rng(seed);
        std::vector<int> order(N);
        std::iota(order.begin(), order.end(), 0);
        std::shuffle(order.begin(), order.end(), rng);

        for (int k = 0; k < N; ++k)
        {
            int pid = order[k];
            players[pid].id = pid;
            players[pid].coupons = 1;
            while (true)
            {
                std::cout << players[pid].getColoredName() << " - place ton point de depart (ligne 1.."
                          << board.size() << ", colonne A..)\n";
                std::cout << "Ligne: ";
                int r = askRow();
                std::cout << "Colonne: ";
                int c = askCol();
                if (board.placeStart(pid, r, c))
                {
                    players[pid].startR = r;
                    players[pid].startC = c;
                    break;
                }
                std::cout << "Impossible ici. Reessaie.\n";
            }
        }

        TileDeck deck = TileDeck::loadBuiltin(N, seed);

        for (int round = 1; round <= rounds; ++round)
        {
            for (int turn = 0; turn < N; ++turn)
            {
                int pid = order[turn];
                Player &P = players[pid];

                auto next5 = deck.peek(5);
                Tile current;
                Hud::printFrame(current, next5, board.size(), players, pid, round);
                std::cout << board.renderForHud(players) << "\n";

                char act = 'T';
                if (!next5.empty())
                {
                    std::cout << "(T)irer";
                    if (P.coupons > 0)
                        std::cout << "  |  (E)changer (coupons: " << P.coupons << ")";
                    std::cout << " ? ";
                    std::cin >> act;
                }
                if ((act == 'E' || act == 'e') && P.coupons > 0)
                {
                    int idx = 0;
                    std::cout << "Indice 0..4 : ";
                    std::cin >> idx;
                    if (idx < 0)
                        idx = 0;
                    if (idx > 4)
                        idx = 4;
                    current = deck.exchangeChoose((size_t)idx);
                    --P.coupons;
                }
                else
                {
                    current = deck.draw();
                }

                next5 = deck.peek(5);
                while (true)
                {
                    Hud::printFrame(current, next5, board.size(), players, pid, round);
                    std::cout << board.renderForHud(players) << "\n";
                    std::cout << "(F)lip, (R)otate, (P)oser, (S)auter ? ";
                    char cmd;
                    std::cin >> cmd;
                    if (cmd == 'F' || cmd == 'f')
                    {
                        current.flipH();
                        continue;
                    }
                    if (cmd == 'R' || cmd == 'r')
                    {
                        current.rotateCW();
                        continue;
                    }
                    if (cmd == 'S' || cmd == 's')
                    {
                        std::cout << "Tuile defaussee.\n";
                        break;
                    }

                    std::cout << "Ligne (coin haut-gauche de la tuile) : ";
                    int r = askRow();
                    std::cout << "Colonne : ";
                    int c = askCol();

                    bool firstTile = false;
                    if (!board.canPlace(current, r, c, pid, firstTile))
                    {
                        std::cout << "Placement invalide (chevauchement/pierre, ne touche pas ton territoire, ou touche un autre joueur).\n";
                        continue;
                    }

                    std::vector<Bonus> bonuses;
                    if (board.placeTile(current, r, c, pid, firstTile, bonuses))
                    {
                        players[pid].cellsOwned = board.countOwned(pid);

                        for (Bonus b : bonuses)
                        {
                            if (b == Bonus::Exchange)
                            {
                                ++P.coupons;
                                std::cout << "Bonus ECHANGE: +1 coupon (=" << P.coupons << ")\n";
                            }
                            else if (b == Bonus::Stone)
                            {
                                std::cout << "Bonus PIERRE: place une pierre (ligne/colonne)\n";
                                int rr, cc;
                                std::cout << "Ligne:";
                                rr = askRow();
                                std::cout << "Colonne:";
                                cc = askCol();
                                if (!board.placeStone(rr, cc))
                                    std::cout << "Position invalide -> pierre perdue.\n";
                            }
                            else if (b == Bonus::Robbery)
                            {
                                std::cout << "Bonus VOL: choisis une case adverse a voler.\n";
                                int victim = -1;
                                for (int k2 = 0; k2 < N; ++k2)
                                    if (order[k2] != pid)
                                    {
                                        victim = order[k2];
                                        break;
                                    }
                                std::cout << "Ligne:";
                                int rr = askRow();
                                std::cout << "Colonne:";
                                int cc = askCol();
                                if (!board.robOneCell(victim, rr, cc, pid))
                                    std::cout << "Impossible de voler ici.\n";
                            }
                        }
                        break;
                    }
                    else
                    {
                        std::cout << "Echec de pose.\n";
                    }
                }
            }
        }

        for (int turn = 0; turn < N; ++turn)
        {
            int pid = order[turn];
            Player &P = players[pid];
            while (P.coupons > 0)
            {
                Hud::printFrame(Tile{}, {}, board.size(), players, pid, rounds);
                std::cout << board.renderForHud(players) << "\n";
                std::cout << P.getColoredName() << " - Acheter une case 1x1 pour 1 coupon ? (O/N) coupons=" << P.coupons << ": ";
                char ch;
                std::cin >> ch;
                if (ch != 'O' && ch != 'o')
                    break;
                std::cout << "Ligne:";
                int r = askRow();
                std::cout << "Colonne:";
                int c = askCol();
                Tile one = Tile::from01Mask(-1, "one", {"1"});
                std::vector<Bonus> dummy;
                if (board.placeTile(one, r, c, pid, false, dummy))
                {
                    --P.coupons;
                }
                else
                {
                    std::cout << "Placement invalide.\n";
                }
            }
        }

        struct Res
        {
            int pid;
            int best;
            int cells;
        };
        std::vector<Res> res;
        for (int pid = 0; pid < N; ++pid)
        {
            int best = board.largestSquareFor(pid);
            int cnt = board.countOwned(pid);
            res.push_back({pid, best, cnt});
            std::cout << players[pid].getColoredName() << " : plus grand carre = "
                      << best << "x" << best << " ; cases = " << cnt << "\n";
        }
        std::sort(res.begin(), res.end(), [](const Res &a, const Res &b)
                  {
        if (a.best!=b.best) return a.best>b.best;
        return a.cells>b.cells; });
        std::cout << "\nGAGNANT : " << players[res[0].pid].getColoredName()
                  << " avec " << res[0].best << "x" << res[0].best
                  << " (cases:" << res[0].cells << ")\n";
    }

    void run(std::vector<Player> &players, Board &board, int rounds, const char *, unsigned seed)
    {
        run(players, board, rounds, seed);
    }

}
