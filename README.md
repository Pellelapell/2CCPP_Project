
# 🎮 Documentation Technique — Projet Jeu de Plateau C++

## 🧩 Sommaire
1. [Introduction](#introduction)
2. [Structure du projet](#structure-du-projet)
3. [Vue d’ensemble](#vue-densemble)
4. [Classes principales](#classes-principales)
   - [Classe `Player`](#classe-player)
   - [Classe `Board`](#classe-board)
   - [Classe `Game`](#classe-game)
5. [Fonctionnement général](#fonctionnement-général)
6. [Diagramme UML textuel](#diagramme-uml-textuel)
7. [Mécanismes de jeu](#mécanismes-de-jeu)
8. [Gestion des bonus](#gestion-des-bonus)
9. [Conditions de victoire](#conditions-de-victoire)
10. [Améliorations possibles](#améliorations-possibles)

---

## 🧠 Introduction

Ce projet est un **jeu de plateau multijoueur en console** développé en **C++**.  
Le joueur choisit son pseudo, sa couleur, puis participe à des **tours de placement de tuiles** sur un plateau (`Board`) en essayant de construire la plus grande zone possible et de profiter des **bonus spéciaux**.

Le projet est structuré en trois grands modules :
- **`Player`** : gestion des joueurs
- **`Board`** : représentation et affichage du plateau
- **`Game`** : logique du jeu (règles, tours, victoire, bonus)

---

## 📁 Structure du projet

```
📦 ProjetJeu
├── main.cpp
├── Player/
│   ├── Player.cpp
│   └── Player.hpp
├── Board/
│   ├── Board.cpp
│   └── Board.hpp
└── Game/
    ├── Game.cpp
    └── Game.hpp
```

**Dépendances :**
- `main.cpp` dépend de `Player` et `Game`
- `Game` dépend de `Player` et `Board`
- `Board` est autonome (sauf dépendances standards `<vector>`, `<random>`, etc.)

---

## 🌐 Vue d’ensemble

Le jeu fonctionne en console :
1. Demande du nombre de joueurs (2 à 9)
2. Saisie des pseudos et couleurs
3. Création du plateau (`Board`)
4. Génération aléatoire des bonus
5. Lancement du jeu via `Game::runGame()`
6. Deux tours principaux :
   - Tour 1 : placement initial (1x1)
   - Tour 2 : placement de tuiles plus grandes, rotations, échanges
7. Gestion des bonus (`Exchange`, `Stone`, `Robbery`)
8. Détermination du gagnant (plus grande zone carrée ou nombre de cases)

---

## 🧍‍♂️ Classe `Player`

**Fichier :** `Player.hpp` / `Player.cpp`  

### 🎯 Rôle
Représente un joueur, son identité, sa couleur et ses coupons d’échange.

### 🧱 Attributs

| Attribut | Type | Description |
|-----------|------|-------------|
| `playerName` | `std::string` | Nom du joueur |
| `playerColor` | `std::string` | Couleur ANSI du joueur |
| `exchangesCoupon` | `int` | Nombre de coupons disponibles |
| `id` | `int` | Identifiant unique du joueur |

### ⚙️ Méthodes

| Méthode | Description |
|----------|-------------|
| `setPlayerName(std::string name)` | Définit le nom du joueur |
| `setPlayerColor(std::string color)` | Définit et colore le joueur |
| `setPlayerId(int id)` | Définit l’ID du joueur |
| `deductCoupon()` | Consomme un coupon |
| `getCoupon()` | Retourne le nombre de coupons |
| `getPlayerName()` / `getPlayerColor()` / `getPlayerId()` | Getters |

---

## 🧩 Classe `Board`

**Fichier :** `Board.hpp` / `Board.cpp`

### 🎯 Rôle
Gère le plateau de jeu, son affichage, la taille et la position des bonus.

### 🧱 Structures associées

```cpp
enum class Bonus { None, Exchange, Stone, Robbery };

struct Cell {
    int used = -1;
    int owner = -1;
    std::string color = "\033[37m";
    std::string symbol = ".";
    Bonus bonus = Bonus::None;
};
```

### ⚙️ Méthodes principales

| Méthode | Description |
|----------|-------------|
| `Board(int size)` | Crée un plateau carré de taille donnée |
| `displayBoard()` | Affiche le plateau et ses bonus |
| `seedBonuses(int numPlayer, std::mt19937& rng)` | Place aléatoirement les bonus |
| `at(int row, int col)` | Accès direct à une case du plateau |

---

## 🎮 Classe `Game`

**Fichier :** `Game.hpp` / `Game.cpp`

### 🎯 Rôle
C’est le **moteur principal du jeu**.  
Il orchestre les tours, gère les placements, les bonus et détermine le vainqueur.

### 🧱 Attributs

| Attribut | Type | Description |
|-----------|------|-------------|
| `players` | `std::vector<Player>` | Liste des joueurs |
| `tiles` | `std::vector<std::vector<std::string>>` | Liste de tuiles |
| `size` | `int` | Taille du plateau |
| `board` | `Board` | Plateau du jeu |
| `exchangeCoupons` | `std::vector<int>` | Gestion des coupons |

### ⚙️ Méthodes principales

| Méthode | Description |
|----------|-------------|
| `Game(std::vector<Player> players, int numPlayers)` | Initialise le jeu |
| `runGame()` | Boucle principale du jeu |
| `initializeTiles()` | Génère et mélange les tuiles |
| `placeTile(...)` | Vérifie et place une tuile |
| `checkForWinner()` | Détermine le gagnant |
| `resolveBonusesAfterPlacement(...)` | Applique les bonus capturés |
| `doRobberyImmediate(...)` | Permet de voler une tuile |
| `placeStoneImmediate(...)` | Place une pierre bloquante |
| `redeemExchangeCoupon()` | Permet de rejouer avec les coupons |

---

## 🧭 Diagramme UML textuel

```
+-----------------+
|     Player      |
+-----------------+
| - id:int        |
| - playerName    |
| - playerColor   |
| - exchangesCoupon:int |
+-----------------+
| +setPlayerName()|
| +setPlayerColor()|
| +deductCoupon() |
| +getCoupon()    |
+-----------------+

         ▲
         |
         |
+-----------------+
|      Game       |
+-----------------+
| - players       |
| - board         |
| - tiles         |
| - exchangeCoupons|
+-----------------+
| +runGame()      |
| +checkForWinner()|
| +placeTile()    |
| +resolveBonuses()|
+-----------------+

         |
         ▼
+-----------------+
|     Board       |
+-----------------+
| - grid:Cell[]   |
| - size:int      |
+-----------------+
| +displayBoard() |
| +seedBonuses()  |
| +at()           |
+-----------------+
```

---

## 🏆 Conditions de victoire

1. Chaque joueur voit sa **plus grande zone carrée** calculée.  
2. En cas d’égalité, le joueur ayant le **plus grand nombre total de cases** gagne.  
3. En cas d’égalité parfaite, la partie est déclarée nulle.

---

## 🚀 Améliorations possibles

- Interface graphique (Qt, SDL, SFML)
- Mode solo avec IA simple
- Sauvegarde et chargement de parties
- Gestion d’un timer de tour
- Personnalisation des couleurs ANSI
