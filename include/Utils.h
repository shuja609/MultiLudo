#pragma once

#include <tuple>
#include <vector>

/** @brief Total number of tokens in play */
extern int numTokens;

/** @brief 2D array storing 3D coordinates for each position on the Ludo board */
extern std::tuple<int, int, int> **LudoGrid;

/** @brief Vector storing dice roll values for the current turn */
extern std::vector<int> diceVal;

/** @brief Counter tracking number of dice rolls in current turn */
extern int diceCount;

/** @brief Current dice roll value */
extern int dice;

/** @brief Flag indicating if player movement is allowed */
extern bool movePlayer;

/** @brief Flag indicating if dice can be rolled */
extern bool moveDice;

/** @brief ID of player whose turn it currently is */
extern int turn;

/** @brief ID of player who had the previous turn */
extern int lastTurn;

/** @brief Vector storing the order of upcoming player turns */
extern std::vector<int> nextTurn;

/** @brief Vector tracking players who have won in order of completion */
extern std::vector<int> winners;

/**
 * @brief Checks if a given board position is a safe spot
 * @param g Tuple containing 3D coordinates of position to check
 * @return true if position is safe, false otherwise
 */
bool isTokenSafe(std::tuple<int, int, int> g);

/**
 * @brief Generates the sequence of player turns
 * Determines turn order based on active players and game rules
 */
void GenerateTurns();

/**
 * @brief Gets the ID of player whose turn is next
 * @return Player ID for next turn
 */
int getTurn();