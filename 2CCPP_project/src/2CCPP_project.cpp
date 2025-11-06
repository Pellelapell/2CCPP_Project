#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cctype>
using namespace std;

class Player {
public:
    string name;
    string color_name;
    string color_code;

    string GetColorCode() {
        string key = color_name;
        std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
            });
        auto it = color_Map.find(key);
        if (it != color_Map.end()) {
            color_code = it->second;
        }
        else {
            color_code = "\033[0m";
        }
        return color_code;
    }

private:
    static const std::unordered_map<std::string, std::string> color_Map;
};

const std::unordered_map<std::string, std::string> Player::color_Map = {
    { "bleu", "\033[34m" },
    { "vert", "\033[32m" },
    { "jaune", "\033[33m" },
    { "orange", "\033[38;5;208m" },
    { "rouge", "\033[31m" },
    { "violet", "\033[35m" },
    { "rose", "\033[38;5;213m" },
    { "blanc", "\033[37m" },
    { "marron", "\033[38;5;94m" }
};

Player GetPlayerInfo();

int main() {
    Player p = GetPlayerInfo();
    cout << p.name << "\n";
    p.GetColorCode();
    cout << p.color_code;
    return 0;
}

Player GetPlayerInfo() {
    Player p;
    cout << "Rentre ton Nom de joueur !\n";
    cin >> p.name;
    while (true) {
        cout << "Rentre la couleur de tes cases !\n"
             << "Bleu, Vert, Jaune, Rouge, Orange, Violet, Rose, Marron, Blanc\n";
        cin >> p.color_name;
        p.GetColorCode();
        if (p.color_code != "\033[0m") {
            break;
        }   else {
        cout << "Couleur non reconnue, réessaye.\n";
    }

    return p;
}
}
