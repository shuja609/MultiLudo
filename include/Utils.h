#pragma once

#include <tuple>
#include <vector>

// Type aliases for better readability
using GridPosition = std::tuple<int, int, int>;
using PlayerList = std::vector<int>;

// Game state variables
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

// Grid position validation
bool isValidPosition(const GridPosition& pos);
bool isValidGridIndex(int gridIndex);
bool isValidRowCol(int row, int col);

// Token movement and safety
bool isTokenSafe(const GridPosition& pos);
bool canTokenMove(const GridPosition& from, const GridPosition& to);
bool isHomePosition(const GridPosition& pos, int playerID);
bool isStartPosition(const GridPosition& pos, int playerID);

// Turn management
void GeneratePlayerTurns();
int getTurn();
bool isPlayerWinner(int playerID);
void addWinner(int playerID);
bool isGameComplete();

// Grid management
bool initializeGridBoard();
void cleanupGridBoard();
GridPosition getGridPosition(int playerID, int tokenID);
void updateGridPosition(int playerID, int tokenID, const GridPosition& newPos);

// Dice management
void resetDiceValue();
void newDiceNumber(int value);
bool isValidDiceRoll(int value);
int getCurrentDiceSum();
bool canRollDice();

// Game state management
void resetcompleteGame();
void saveGameState();
void loadGameState();
bool isValidGameState();

// Player position calculations
GridPosition calculateNextPosition(const GridPosition& current, int steps);
bool isPathClear(const GridPosition& from, const GridPosition& to);
int calculateDistance(const GridPosition& from, const GridPosition& to); 