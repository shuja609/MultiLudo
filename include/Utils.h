#pragma once

#include <tuple>
#include <vector>

extern int numTokens;
extern std::tuple<int, int, int> **LudoGrid;
extern std::vector<int> diceVal;
extern int diceCount;
extern int dice;
extern bool movePlayer;
extern bool moveDice;
extern int turn;
extern int lastTurn;
extern std::vector<int> nextTurn;
extern std::vector<int> winners;

bool isTokenSafe(std::tuple<int, int, int> g);
void GenerateTurns();
int getTurn(); 