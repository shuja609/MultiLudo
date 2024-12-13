#include "../include/Utils.h"
#include <algorithm>
#include <random>

// Global game state variables
/** Number of tokens in play, initialized to -1 */
int numTokens = -1;

/** 2D array storing 3D coordinates for board positions */
std::tuple<int, int, int> **LudoGrid;

/** Vector storing last 3 dice rolls */
std::vector<int> diceVal(3, 0);

/** Counter for number of dice rolls in current turn */
int diceCount = 0;

/** Current dice roll value */
int dice = 1;

/** Flag controlling if player movement is allowed */
bool movePlayer = false;

/** Flag controlling if dice can be rolled */
bool moveDice = true;

/** ID of player whose turn it currently is (1-4) */
int turn = 1;

/** ID of player who had the previous turn */
int lastTurn = turn;

/** Vector storing upcoming player turn order */
std::vector<int> nextTurn;

/** Vector tracking players who have won in order of completion */
std::vector<int> winners;

/**
 * @brief Determines if a given board position is a safe spot
 * 
 * Safe spots are special positions on the board where tokens cannot be captured.
 * These are located at coordinates (i,2,1) and (i,0,3) for each player i.
 *
 * @param g Tuple containing 3D coordinates (x,y,z) of position to check
 * @return true if position is a safe spot, false otherwise
 */
bool isTokenSafe(std::tuple<int, int, int> g) {
    for (int i = 0; i < 4; i++) {
        if (g == std::make_tuple(i, 2, 1) || g == std::make_tuple(i, 0, 3))
            return true;
    }
    return false;
}

/**
 * @brief Generates a random sequence of player turns
 * 
 * Creates a vector of 4 unique random numbers between 1-4 representing
 * the order in which players will take their turns. This ensures fair
 * and random turn distribution among players.
 */
void GenerateTurns() {
    nextTurn.resize(4);
    int r = (rand() % 4) + 1;
    int count = 0;
    while (count != 4) {
        if (std::find(nextTurn.begin(), nextTurn.end(), r) != nextTurn.end()) {
            r = (rand() % 4) + 1;
        } else {
            nextTurn[count] = r;
            count++;
        }
    }
}

/**
 * @brief Determines and returns the ID of the next player's turn
 * 
 * This function manages the turn order by:
 * 1. Generating new turn sequence if needed
 * 2. Taking the next player ID from the sequence
 * 3. Skipping players who have already won
 * 4. Regenerating turn sequence if exhausted
 *
 * @return int Player ID (1-4) whose turn is next
 */
int getTurn() {
    int t;
    if (nextTurn.empty()) {
        GenerateTurns();
        t = nextTurn[nextTurn.size() - 1];
        nextTurn.pop_back();
        while (std::count(winners.begin(), winners.end(), t)) {
            t = nextTurn[nextTurn.size() - 1];
            nextTurn.pop_back();
        }
        return t;
    } else {
        t = nextTurn[nextTurn.size() - 1];
        nextTurn.pop_back();
        while (std::count(winners.begin(), winners.end(), t)) {
            if (nextTurn.empty()) {
                GenerateTurns();
                t = nextTurn[nextTurn.size() - 1];
                nextTurn.pop_back();
            } else {
                t = nextTurn[nextTurn.size() - 1];
                nextTurn.pop_back();
            }
        }
    }
    return t;
}