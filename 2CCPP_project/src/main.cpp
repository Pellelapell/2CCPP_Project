#include <iostream>
#include <string>
#include <algorithm>
#include "Player.hpp"
#include <vector>
using namespace std;

bool isDigit(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}
int askNumPlayers() 
{
    int numPlayers;
    std::string input;
    while(true)
    {
    std::cout <<"Combien de joueurs participeront, entre 1 et 9" << std::endl;
    std::cin >> input;
    while (isDigit(input)!=true)
    {
        std::cout << "Nombre de joueurs invalide" << std::endl;
        std::cout <<"Combien de joueurs participeront, entre 1 et 9" << std::endl;
        std::cin >> input;
    }
    numPlayers  = std::stoi(input);
    if (input.size()!=1||numPlayers < 1) 
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

string askColorPlayer(){
    string color;
    std::cout << "Entre la couleur de tes cases : bleu, vert, jaune, orange, rouge, violet, rose, blanc, marron" << std::endl;
    std::cin >> color;
    if (color != "bleu" && color != "vert" && color != "jaune" && color != "orange" && color != "rouge" && color != "violet" && color != "rose" && color != "blanc" && color != "marron" && color != "BLEU" && color != "VERT" && color != "JAUNE" && color != "ORANGE" && color != "ROUGE" && color != "VIOLET" && color != "ROSE" && color != "BLANC" && color != "MARRON" && color != "Bleu" && color != "Vert" && color != "Jaune" && color != "Orange" && color != "Rouge" && color != "Violet" && color != "Rose" && color != "Blanc" && color != "Marron")
    {
        std::cout << "Couleur invalide, choisis parmi : bleu, vert, jaune, orange, rouge, violet, rose, blanc, marron" << std::endl;
        askColorPlayer();
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
        askColorPlayer() = p.colorName;
        players.push_back(p);
    }
    std::cout << players[3].name << std::endl;
    std::cout << players[3].colorName << std::endl;
    return players;
}

int main() 
{
    createPlayers(askNumPlayers());
    return 0;
}

