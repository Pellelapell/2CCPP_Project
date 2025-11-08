#include <iostream>
#include <string>
#include <algorithm>
#include "Player.hpp"
#include "Board.hpp"
#include <vector>
using namespace std;

vector<string> availableColors =
    {
        "rouge", "bleu", "vert", "jaune", "orange", "violet", "rose", "marron", "blanc"};
bool isDigit(const std::string &s)
{
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

string toLower(string s)
{
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c)
              { return tolower(c); });
    return s;
}

void displayAvailableColors(const vector<string> &colors)
{

    std::cout << "Couleurs disponibles : ";
    for (const auto &c : colors)
    {
        std::cout << c << " ";
    }
    std::cout << "\n"
              << std::endl;
}
int askNumPlayers()
{
    int numPlayers;
    std::string input;
    while (true)
    {
        std::cout << "Combien de joueurs participeront, entre 1 et 9" << std::endl;
        std::cin >> input;
        while (isDigit(input) != true)
        {
            std::cout << "Nombre de joueurs invalide" << std::endl;
            std::cout << "Combien de joueurs participeront, entre 1 et 9" << std::endl;
            std::cin >> input;
        }
        numPlayers = std::stoi(input);
        if (input.size() != 1 || numPlayers < 1)
        {
            std::cout << "Nombre de joueurs invalide" << std::endl;
        }
        else
        {
            std::cout << "Nombre de joueurs: " << numPlayers << std::endl;
            break;
        }
    }
    return numPlayers;
}

string askColorPlayer()
{
    displayAvailableColors(availableColors);
    string color;
    std::cin >> color;
    if (color != "bleu" && color != "vert" && color != "jaune" && color != "orange" && color != "rouge" && color != "violet" && color != "rose" && color != "blanc" && color != "marron" && color != "BLEU" && color != "VERT" && color != "JAUNE" && color != "ORANGE" && color != "ROUGE" && color != "VIOLET" && color != "ROSE" && color != "BLANC" && color != "MARRON" && color != "Bleu" && color != "Vert" && color != "Jaune" && color != "Orange" && color != "Rouge" && color != "Violet" && color != "Rose" && color != "Blanc" && color != "Marron" && color != "Marron")
    {
        std::cout << "Couleur invalide, choisis parmi les couleurs disponibles !" << std::endl;
        return askColorPlayer();
    }
    auto it = find_if(availableColors.begin(), availableColors.end(),
                      [&](const string &c)
                      { return toLower(c) == toLower(color); });
    if (it == availableColors.end())
    {
        cout << "Couleur deja prise, choisis une autre couleur." << std::endl;
        return askColorPlayer();
    }
    else
    {
        availableColors.erase(it);
        return color;
    }

    return color;
}

vector<Player> createPlayers(int numPlayers)
{

    vector<Player> players;

    for (int i = 0; i < numPlayers; ++i)
    {
        Player p;
        cout << "\n=== Joueur " << (i + 1) << " ===" << endl;
        cout << "Entre ton nom : ";
        cin >> p.name;
        p.colorName = askColorPlayer();
        p.assignColorToColorName();
        players.push_back(p);
    }
    return players;
    cout << "\nTous les joueurs ont été créés !" << endl;
}

int main()
{
    int numPlayers = askNumPlayers();
    vector playerVector = createPlayers(numPlayers);
    Board board(numPlayers);
    cout << "\nVoici le plateau de jeu :" << endl;
    board.display();
    cout << "\n=== Liste des joueurs et leurs couleurs ===" << endl;
    for (int i = 0; i < playerVector.size(); i++)
    {
        Player p = playerVector[i];
        cout << p.getColoredName() << endl;
    }
    return 0;
}
