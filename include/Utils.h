#pragma once

#include <tuple>
#include <vector>

/**
 * @brief Type aliases for improved code readability and maintenance
 */
using GridPosition = std::tuple<int, int, int>;  ///< Represents a 3D position (x,y,z) on the game grid
using PlayerList = std::vector<int>;             ///< List of player IDs

/**
 * @brief External game state variables shared across the application
 */
extern int numTokens;                            ///< Number of tokens per player
extern std::tuple<int, int, int> **LudoGrid;     ///< 2D grid representing the game board state
extern std::vector<int> diceVal;                 ///< Current dice roll values
extern int diceCount;                           ///< Number of dice rolls made
extern int dice;                                ///< Current dice value
extern bool movePlayer;                         ///< Flag indicating if player can move
extern bool moveDice;                           ///< Flag indicating if dice can be rolled
extern int turn;                                ///< Current player's turn
extern int lastTurn;                            ///< Previous player's turn
extern std::vector<int> nextTurn;               ///< Queue of upcoming player turns
extern std::vector<int> winners;                ///< List of players who have won

/**
 * @brief Grid position validation functions
 */
bool isValidPosition(const GridPosition& pos);    ///< Checks if a position is within grid bounds
bool isValidGridIndex(int gridIndex);            ///< Validates grid array index
bool isValidRowCol(int row, int col);            ///< Checks if row/column coordinates are valid

/**
 * @brief Token movement and safety checking functions
 */
bool isTokenSafe(const GridPosition& pos);        ///< Determines if a token is in a safe position
bool canTokenMove(const GridPosition& from, const GridPosition& to);  ///< Checks if token movement is legal
bool isHomePosition(const GridPosition& pos, int playerID);  ///< Checks if position is player's home
bool isStartPosition(const GridPosition& pos, int playerID); ///< Checks if position is player's start

/**
 * @brief Turn management and game progression functions
 */
void GeneratePlayerTurns();                      ///< Creates the turn order for players
int getTurn();                                   ///< Returns the current player's turn
bool isPlayerWinner(int playerID);               ///< Checks if specified player has won
void addWinner(int playerID);                    ///< Adds player to winners list
bool isGameComplete();                           ///< Checks if game has ended

/**
 * @brief Grid management and manipulation functions
 */
bool initializeGridBoard();                      ///< Sets up initial game board state
void cleanupGridBoard();                         ///< Frees grid resources
GridPosition getGridPosition(int playerID, int tokenID);  ///< Gets position of player's token
void updateGridPosition(int playerID, int tokenID, const GridPosition& newPos);  ///< Updates token position

/**
 * @brief Dice rolling and validation functions
 */
void resetDiceValue();                           ///< Resets dice to initial state
void newDiceNumber(int value);                   ///< Sets new dice roll value
bool isValidDiceRoll(int value);                 ///< Validates dice roll value
int getCurrentDiceSum();                         ///< Gets sum of current dice values
bool canRollDice();                             ///< Checks if dice roll is allowed

/**
 * @brief Game state persistence and validation functions
 */
void resetcompleteGame();                        ///< Resets game to initial state
void saveGameState();                            ///< Persists current game state
void loadGameState();                            ///< Loads saved game state
bool isValidGameState();                         ///< Validates loaded game state

/**
 * @brief Position calculation and path finding functions
 */
GridPosition calculateNextPosition(const GridPosition& current, int steps);  ///< Calculates next valid position
bool isPathClear(const GridPosition& from, const GridPosition& to);  ///< Checks if path is unobstructed
int calculateDistance(const GridPosition& from, const GridPosition& to);  ///< Calculates distance between positions