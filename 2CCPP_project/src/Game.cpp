#include <bits/stdc++.h>
#include "Board.hpp"
#include "Tile.hpp"
#include "Player.hpp"

using namespace std;

enum class Bonus
{
    Exchange,
    Stone,
    Robbery
};

static std::mt19937 &rng()
{
    static std::mt19937 gen(1337u);
    return gen;
}

static int askInt(const string &prompt, int lo, int hi)
{
    while (true)
    {
        cout << prompt;
        string s;
        if (!(cin >> s))
            exit(0);
        try
        {
            int v = stoi(s);
            if (v >= lo && v <= hi)
                return v;
        }
        catch (...)
        {
        }
        cout << "  Entrez un entier dans [" << lo << "," << hi << "].\n";
    }
}

static bool yesno(const string &prompt, bool def = false)
{
    cout << prompt << (def ? " [Y/n] " : " [y/N] ");
    string s;
    cin >> s;
    if (s.empty())
        return def;
    char c = tolower(s[0]);
    if (c == 'y')
        return true;
    if (c == 'n')
        return false;
    return def;
}

static pair<int, int> askXY(const string &prompt)
{
    cout << prompt;
    int x, y;
    cin >> x >> y;
    return {x, y};
}

struct PlacedTile
{
    Tile tile;
    int rot;
    bool flip;
    int ox, oy;
    vector<pair<int, int>> absCells() const
    {
        vector<pair<int, int>> cs = tile.orientedCells(rot, flip);
        for (auto &p : cs)
        {
            p.first += ox;
            p.second += oy;
        }
        return cs;
    }
};

class Game
{
public:
    void run();

private:
    void setupPlayers();
    void setupBoardAndBonuses();
    void setupStarts();
    void buildDeck();

    void roundLoop();
    Tile drawWithCouponOption(Player &p);
    bool placeTileInteractive(int pid, const Tile &t, PlacedTile *outPlaced = nullptr);

    void checkAndApplyBonuses(int pid);
    void applyExchange(int pid) { coupons[pid]++; }
    void applyStone(int pid);
    void applyRobbery(int pid);

    void finalMonoTiles();
    void scoreAndPrint();

    bool isOwnedBy(int x, int y, int pid) const { return board.at(x, y) == pid; }
    bool emptyAt(int x, int y) const { return board.at(x, y) == -1; }
    bool stoneAt(int x, int y) const { return board.at(x, y) == -2; }
    int pidCouponsIndex(int pid) const { return pid; }
    void renderWithBonuses() const;
    void placeBonusMarkersOnRenderGrid(vector<string> &lines) const;
    bool surroundedByNSWE(int cx, int cy, int pid) const;
    void captureBonusCell(int cx, int cy, int pid);

    vector<Tile> makeCatalog() const;

private:
    vector<Player> players;
    vector<int> coupons;
    vector<vector<PlacedTile>> placedByPlayer;

    Board board = Board(20, 20);
    int W = 20, H = 20;

    map<pair<int, int>, Bonus> bonuses;
    set<pair<int, int>> claimed;

    deque<Tile> mainDeck;
    vector<Tile> deferred;

    int maxRounds = 9;

    vector<pair<int, int>> starts;
};

void Game::run()
{
    setupPlayers();
    setupBoardAndBonuses();
    setupStarts();
    buildDeck();
    roundLoop();
    finalMonoTiles();
    scoreAndPrint();
    cout << "\nFin de partie. Merci d'avoir joué !\n";
}

void Game::setupPlayers()
{
    cout << "\n=== Laying Grass (CLI ASCII) ===\n";
    int n = askInt("Nombre de joueurs (2-9) ? ", 2, 9);
    players.clear();
    coupons.assign(n, 1);
    placedByPlayer.assign(n, {});
    starts.assign(n, {-1, -1});

    for (int i = 0; i < n; ++i)
    {
        cout << "Nom du joueur " << (i + 1) << " : ";
        string name;
        cin >> ws;
        getline(cin, name);
        if (name.empty())
            name = string("J") + to_string(i + 1);
        players.emplace_back(i, name);
    }
    shuffle(players.begin(), players.end(), rng());
    vector<int> newCoupons(n, 1);
    vector<vector<PlacedTile>> newPlaced(n);
    vector<pair<int, int>> newStarts(n, {-1, -1});
    for (int i = 0; i < n; ++i)
    {
        newCoupons[i] = 1;
    }
    coupons.swap(newCoupons);
    placedByPlayer.swap(newPlaced);
    starts.swap(newStarts);

    cout << "\nOrdre de jeu :\n";
    for (int i = 0; i < n; ++i)
        cout << "  " << (i + 1) << ". " << players[i].name() << "\n";
}

void Game::setupBoardAndBonuses()
{
    int n = (int)players.size();
    if (n <= 4)
    {
        W = 20;
        H = 20;
    }
    else
    {
        W = 30;
        H = 30;
    }
    board = Board(W, H);
    bonuses.clear();
    claimed.clear();

    int nExchange = (int)ceil(1.5 * n);
    int nStone = (int)ceil(0.5 * n);
    int nRob = n;
    int total = nExchange + nStone + nRob;

    auto canPlaceBonus = [&](int x, int y)
    {
        if (x <= 0 || y <= 0 || x >= W - 1 || y >= H - 1)
            return false;
        if (bonuses.count({x, y}))
            return false;
        static const int dx[4] = {1, -1, 0, 0};
        static const int dy[4] = {0, 0, 1, -1};
        for (int k = 0; k < 4; ++k)
        {
            int nx = x + dx[k], ny = y + dy[k];
            if (bonuses.count({nx, ny}))
                return false;
        }
        return true;
    };

    vector<Bonus> bag;
    bag.insert(bag.end(), nExchange, Bonus::Exchange);
    bag.insert(bag.end(), nStone, Bonus::Stone);
    bag.insert(bag.end(), nRob, Bonus::Robbery);
    shuffle(bag.begin(), bag.end(), rng());

    int placed = 0, tries = 0, maxTries = 100000;
    while (placed < total && tries < maxTries)
    {
        ++tries;
        int x = int(rng()() % W), y = int(rng()() % H);
        if (!canPlaceBonus(x, y))
            continue;
        bonuses[{x, y}] = bag[placed++];
    }
    if (placed < total)
    {
        cout << "Avertissement: tous les bonus n'ont pas pu être placés.\n";
    }
}

void Game::setupStarts()
{
    cout << "\n--- Placement des cases de départ (1x1) ---\n";
    for (int i = 0; i < (int)players.size(); ++i)
    {
        while (true)
        {
            auto [x, y] = askXY("  " + players[i].name() + " — x y ? ");
            if (!board.inBounds(x, y))
            {
                cout << "  Hors plateau.\n";
                continue;
            }
            if (board.at(x, y) != -1)
            {
                cout << "  Case occupée.\n";
                continue;
            }
            bool bad = false;
            static const int dx[4] = {1, -1, 0, 0};
            static const int dy[4] = {0, 0, 1, -1};
            for (int k = 0; k < 4; ++k)
            {
                int nx = x + dx[k], ny = y + dy[k];
                if (board.inBounds(nx, ny) && board.at(nx, ny) >= 0)
                {
                    bad = true;
                    break;
                }
            }
            if (bad)
            {
                cout << "  Trop proche d'un autre départ.\n";
                continue;
            }
            board.setCell(x, y, players[i].id());
            starts[i] = {x, y};
            break;
        }
        renderWithBonuses();
    }
}

vector<Tile> Game::makeCatalog() const
{

    auto T = [&](initializer_list<pair<int, int>> cs)
    { return Tile(vector<pair<int, int>>(cs)); };
    vector<Tile> C = {
        T({{0, 0}}),
        T({{0, 0}, {1, 0}}),
        T({{0, 0}, {1, 0}, {2, 0}}),
        T({{0, 0}, {0, 1}, {1, 1}}),
        T({{0, 0}, {1, 0}, {0, 1}, {1, 1}}),
        T({{0, 0}, {1, 0}, {2, 0}, {1, 1}}),
        T({{0, 0}, {1, 0}, {2, 0}, {3, 0}}),
        T({{0, 0}, {1, 0}, {0, 1}, {0, 2}}),
        T({{1, 0}, {0, 1}, {1, 1}, {2, 1}}),
        T({{1, 0}, {2, 0}, {0, 1}, {1, 1}}),
        T({{0, 0}, {1, 0}, {1, 1}, {2, 1}}),
        T({{0, 0}, {0, 1}, {0, 2}, {1, 2}}),
        T({{0, 0}, {1, 0}, {2, 0}, {2, 1}}),
        T({{0, 0}, {0, 1}, {1, 1}, {2, 1}, {2, 2}}),
        T({{0, 0}, {1, 0}, {2, 0}, {1, 1}, {1, 2}}),
        T({{0, 0}, {1, 0}, {2, 0}, {0, 1}, {2, 1}}),
        T({{0, 0}, {1, 0}, {1, 1}, {1, 2}, {2, 2}}),
        T({{0, 0}, {1, 0}, {2, 0}, {3, 0}, {1, 1}}),
        T({{0, 0}, {1, 0}, {2, 0}, {0, 1}, {0, 2}}),
        T({{1, 0}, {0, 1}, {1, 1}, {2, 1}, {1, 2}}),
    };
    return C;
}

void Game::buildDeck()
{
    int n = (int)players.size();
    int want = (int)round(10.67 * n);

    auto catalog = makeCatalog();
    vector<Tile> bag;
    while ((int)bag.size() < want)
    {
        bag.insert(bag.end(), catalog.begin(), catalog.end());
    }
    bag.resize(want);

    shuffle(bag.begin(), bag.end(), rng());
    mainDeck.clear();
    for (auto &t : bag)
        mainDeck.push_back(t);
    deferred.clear();
}

Tile Game::drawWithCouponOption(Player &p)
{
    int pid = p.id();
    if (mainDeck.empty() && !deferred.empty())
    {
        cout << "  Deck épuisé. Réintroduction des tuiles différées...\n";
        for (auto &t : deferred)
            mainDeck.push_back(t);
        deferred.clear();
    }
    if (mainDeck.empty())
    {
        cout << "  Plus de tuiles.\n";
        return Tile({{0, 0}});
    }

    vector<Tile> preview;
    {
        int k = min(5, (int)mainDeck.size());
        auto it = mainDeck.begin();
        for (int i = 0; i < k; ++i, ++it)
            preview.push_back(*it);
    }
    cout << "  Prochaines tuiles (5 max):\n";
    for (int i = 0; i < (int)preview.size(); ++i)
    {
        cout << "   [" << i << "] size=" << (int)preview[i].cells().size() << "\n";
    }

    Tile chosen = mainDeck.front();
    if (coupons[pid] > 0 && preview.size() > 1)
    {
        bool use = yesno("  Utiliser un coupon pour choisir parmi ces 5 ?", false);
        if (use)
        {
            int pick = askInt("   Choisir l'index (0.." + to_string((int)preview.size() - 1) + ") : ", 0, (int)preview.size() - 1);
            chosen = preview[pick];
            for (int i = 0; i < pick; ++i)
            {
                deferred.push_back(preview[i]);
            }
            for (int i = 0; i <= pick; ++i)
                mainDeck.pop_front();
            coupons[pid]--;
            cout << "  Coupon consommé. Tuile choisie de taille " << (int)chosen.cells().size() << ".\n";
            return chosen;
        }
    }
    chosen = mainDeck.front();
    mainDeck.pop_front();
    cout << "  Tuile tirée de taille " << (int)chosen.cells().size() << ".\n";
    return chosen;
}

bool Game::placeTileInteractive(int pid, const Tile &t, PlacedTile *outPlaced)
{
    board.render(cout);
    cout << "\nTuile à jouer (aperçu):\n"
         << t << "\n";

    int rot = 0;
    cout << "Rotation (0/90/180/270) ? ";
    cin >> rot;
    if (!(rot == 0 || rot == 90 || rot == 180 || rot == 270))
        rot = 0;

    char f = 'n';
    cout << "Flip horizontal (y/n) ? ";
    cin >> f;
    bool flip = (f == 'y' || f == 'Y');

    int ox = 0, oy = 0;
    while (true)
    {
        tie(ox, oy) = askXY("Origine X Y ? ");
        if (board.canPlace(t, rot, flip, ox, oy, players[pid].id()))
        {
            board.place(t, rot, flip, ox, oy, players[pid].id());
            if (outPlaced)
                *outPlaced = PlacedTile{t, rot, flip, ox, oy};
            break;
        }
        else
        {
            cout << "  Placement invalide. Réessaie.\n";
        }
    }
    board.render(cout);
    return true;
}

bool Game::surroundedByNSWE(int cx, int cy, int pid) const
{
    static const int dx[4] = {0, 0, -1, 1};
    static const int dy[4] = {-1, 1, 0, 0};
    for (int k = 0; k < 4; ++k)
    {
        int nx = cx + dx[k], ny = cy + dy[k];
        if (!board.inBounds(nx, ny))
            return false;
        if (board.at(nx, ny) != players[pid].id())
            return false;
    }
    return true;
}

void Game::captureBonusCell(int cx, int cy, int pid)
{
    if (board.at(cx, cy) == -1)
        board.setCell(cx, cy, players[pid].id());
    claimed.insert({cx, cy});
}

void Game::checkAndApplyBonuses(int pid)
{
    vector<pair<pair<int, int>, Bonus>> newly;
    for (auto &kv : bonuses)
    {
        auto c = kv.first;
        if (claimed.count(c))
            continue;
        if (surroundedByNSWE(c.first, c.second, pid))
        {
            newly.push_back(kv);
        }
    }
    for (auto &it : newly)
    {
        auto [cell, bt] = it;
        cout << "  >> Bonus capturé en (" << cell.first << "," << cell.second << "): ";
        if (bt == Bonus::Exchange)
        {
            cout << "EXCHANGE (+1 coupon)\n";
            coupons[pid] += 1;
            captureBonusCell(cell.first, cell.second, pid);
        }
        else if (bt == Bonus::Stone)
        {
            cout << "STONE (placer une pierre)\n";
            captureBonusCell(cell.first, cell.second, pid);
            applyStone(pid);
        }
        else if (bt == Bonus::Robbery)
        {
            cout << "ROBBERY (voler une tuile)\n";
            captureBonusCell(cell.first, cell.second, pid);
            applyRobbery(pid);
        }
    }
}

void Game::applyStone(int pid)
{
    cout << "    Placer une pierre (#). Donne des coordonnées.\n";
    while (true)
    {
        auto [x, y] = askXY("    Pierre x y ? ");
        if (!board.inBounds(x, y))
        {
            cout << "    Hors plateau.\n";
            continue;
        }
        if (board.at(x, y) != -1)
        {
            cout << "    Case non vide.\n";
            continue;
        }
        board.setCell(x, y, -2);
        cout << "    Pierre placée en (" << x << "," << y << ").\n";
        board.render(cout);
        break;
    }

    if (coupons[pid] > 0 && yesno("    Retirer une pierre pour 1 coupon ?", false))
    {
        auto [x, y] = askXY("    Retrait pierre x y ? ");
        if (board.inBounds(x, y) && board.at(x, y) == -2)
        {
            board.setCell(x, y, -1);
            coupons[pid]--;
            cout << "    Pierre retirée.\n";
            board.render(cout);
        }
        else
        {
            cout << "    Pas de pierre à cet endroit.\n";
        }
    }
}

void Game::applyRobbery(int pid)
{
    struct Opt
    {
        int victim;
        int index;
        int sz;
    };
    vector<Opt> opts;
    for (int v = 0; v < (int)players.size(); ++v)
    {
        if (v == pid)
            continue;
        for (int i = 0; i < (int)placedByPlayer[v].size(); ++i)
        {
            int sz = (int)placedByPlayer[v][i].tile.cells().size();
            opts.push_back({v, i, sz});
        }
    }
    if (opts.empty())
    {
        cout << "    Aucun pion adverse à voler.\n";
        return;
    }
    sort(opts.begin(), opts.end(), [](const Opt &a, const Opt &b)
         {
    if (a.sz!=b.sz) return a.sz>b.sz; 
    if (a.victim!=b.victim) return a.victim<b.victim;
    return a.index<b.index; });

    cout << "    Options de vol (victime, idx, taille):\n";
    for (int i = 0; i < (int)min<size_t>(opts.size(), 10); ++i)
    {
        cout << "     [" << i << "] " << players[opts[i].victim].name()
             << ", tile#" << opts[i].index << ", size=" << opts[i].sz << "\n";
    }
    int pick = askInt("    Choisir index 0.." + to_string((int)opts.size() - 1) + " (ou 0 si une seule) : ",
                      0, (int)opts.size() - 1);
    auto chosen = opts[pick];

    auto stolen = placedByPlayer[chosen.victim][chosen.index];
    auto cells = stolen.absCells();
    for (auto &c : cells)
    {
        if (board.inBounds(c.first, c.second) && board.at(c.first, c.second) == players[chosen.victim].id())
            board.setCell(c.first, c.second, -1);
    }
    placedByPlayer[chosen.victim].erase(placedByPlayer[chosen.victim].begin() + chosen.index);

    cout << "    Tuile volée. Place-la immédiatement.\n";
    PlacedTile newpt;
    bool ok = placeTileInteractive(pid, stolen.tile, &newpt);
    if (!ok)
    {
        cout << "    Placement impossible : vol perdu (la tuile disparaît).\n";
    }
    else
    {
        placedByPlayer[pid].push_back(newpt);
    }
}

void Game::renderWithBonuses() const
{
    board.render(cout);
    cout << "Bonus restants (* en console):\n";
    for (auto &kv : bonuses)
    {
        if (claimed.count(kv.first))
            continue;
        auto [x, y] = kv.first;
        cout << "  - (" << x << "," << y << ") : ";
        if (kv.second == Bonus::Exchange)
            cout << "EXCHANGE";
        else if (kv.second == Bonus::Stone)
            cout << "STONE";
        else
            cout << "ROBBERY";
        cout << "\n";
    }
}

void Game::roundLoop()
{
    cout << "\n=== Début de partie : 9 tours ===\n";
    int n = (int)players.size();
    for (int round = 1; round <= maxRounds; ++round)
    {
        cout << "\n===== Tour " << round << "/" << maxRounds << " =====\n";
        for (int i = 0; i < n; ++i)
        {
            cout << "\n-- Au tour de " << players[i].name()
                 << " (coupons=" << coupons[i] << ") --\n";
            if (mainDeck.empty() && !deferred.empty())
            {
                for (auto &t : deferred)
                    mainDeck.push_back(t);
                deferred.clear();
            }
            if (mainDeck.empty())
            {
                cout << "  Plus de tuiles.\n";
                continue;
            }

            Tile t = drawWithCouponOption(players[i]);

            PlacedTile pt;
            bool ok = placeTileInteractive(i, t, &pt);
            if (!ok)
            {
                cout << "  Tuile défaussée (aucun placement valide).\n";
            }
            else
            {
                placedByPlayer[i].push_back(pt);
            }

            checkAndApplyBonuses(i);

            renderWithBonuses();
        }
    }
}

void Game::finalMonoTiles()
{
    cout << "\n=== Phase finale : achat de 1x1 ===\n";
    Tile mono({{0, 0}});
    for (int i = 0; i < (int)players.size(); ++i)
    {
        if (coupons[i] <= 0)
            continue;
        cout << " " << players[i].name() << " a " << coupons[i] << " coupon(s).\n";
        int k = askInt("  Combien de 1x1 acheter (0.." + to_string(coupons[i]) + ") ? ", 0, coupons[i]);
        for (int j = 0; j < k; ++j)
        {
            while (true)
            {
                int rot = 0;
                bool flip = false;
                auto [x, y] = askXY("   Poser 1x1 x y ? ");
                if (!board.inBounds(x, y))
                {
                    cout << "   Hors plateau.\n";
                    continue;
                }
                if (!board.canPlace(mono, rot, flip, x, y, players[i].id()))
                {
                    cout << "   Position invalide (contact, collision ou hors règle).\n";
                    continue;
                }
                board.place(mono, rot, flip, x, y, players[i].id());
                coupons[i]--;
                board.render(cout);
                break;
            }
        }
    }
}

void Game::scoreAndPrint()
{
    cout << "\n=== Scores ===\n";
    int n = (int)players.size();

    vector<int> area(n, 0);
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
        {
            int v = board.at(x, y);
            if (v >= 0)
            {
                area[v]++;
            }
        }
    auto largestSquareFor = [&](int pid) -> int
    {
        vector<vector<int>> dp(H + 1, vector<int>(W + 1, 0));
        int best = 0;
        for (int y = 1; y <= H; ++y)
            for (int x = 1; x <= W; ++x)
            {
                if (board.at(x - 1, y - 1) == players[pid].id())
                {
                    dp[y][x] = 1 + min({dp[y - 1][x], dp[y][x - 1], dp[y - 1][x - 1]});
                    best = max(best, dp[y][x]);
                }
            }
        return best;
    };

    struct R
    {
        int sq;
        int ar;
        int pid;
    };
    vector<R> res;
    for (int i = 0; i < n; ++i)
    {
        int sq = largestSquareFor(i);
        cout << " " << players[i].name() << " : plus grand carre " << sq << "x" << sq
             << ", aire=" << area[players[i].id()] << "\n";
        res.push_back({sq, area[players[i].id()], i});
    }
    sort(res.begin(), res.end(), [](const R &a, const R &b)
         {
    if (a.sq!=b.sq) return a.sq>b.sq;
    if (a.ar!=b.ar) return a.ar>b.ar;
    return a.pid<b.pid; });

    int bestSq = res.front().sq;
    int bestAr = res.front().ar;
    vector<int> winners;
    for (auto &r : res)
        if (r.sq == bestSq && r.ar == bestAr)
            winners.push_back(r.pid);

    if (winners.size() > 1)
    {
        cout << "Égalité parfaite, gagnants :\n";
        for (int pid : winners)
            cout << " - " << players[pid].name() << "\n";
    }
    else
    {
        int w = winners.front();
        cout << "Vainqueur : " << players[w].name()
             << " avec " << bestSq << "x" << bestSq << " (aire " << bestAr << ").\n";
    }
}
