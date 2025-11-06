#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

bool isDigit(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

int main() {
    std::string input;
    std::cout <<"Combien de joueurs participeront, entre 1 et 9" << std::endl;
    std::cin >> input;
    while (isDigit(input)!=true)
    {
        std::cout << "Nombre de joueurs invalide" << std::endl;
        std::cout <<"Combien de joueurs participeront, entre 1 et 9" << std::endl;
        std::cin >> input;
    }
    int numPlayers  = std::stoi(input);
    if (input.size()!=1||numPlayers < 1) 
    {
        std::cout << "Nombre de joueurs invalide" << std::endl;
    }
    else 
    {
        std::cout << "Nombre de joueurs: " << numPlayers << std::endl;
    }
    return 0;
}