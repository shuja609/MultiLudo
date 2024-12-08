/**
 * @file Utils.cpp
 * @brief Implementation of utility functions and game state management for MultiLudo
 */

#include "../include/Utils.h"
#include <algorithm>
#include <random>
#include <stdexcept>
#include <iostream>
#include <numeric>

//------------------------------------------------------------------------------
// Global Game State Variables
//------------------------------------------------------------------------------
int numTokens = -1;                              ///< Number of tokens per player (-1 indicates uninitialized)
std::tuple<int, int, int> **LudoGrid = nullptr; ///< Dynamic 2D array storing token positions
std::vector<int> diceVal(3, 0);                 ///< Stores up to 3 dice roll values
int diceCount = 0;                              ///< Number of dice rolls made in current turn
int dice = 1;                                   ///< Current dice value
bool movePlayer = false;                        ///< Flag indicating if player movement is allowed
bool moveDice = true;                          ///< Flag indicating if dice rolling is allowed
int turn = 1;                                  ///< Current player's turn (1-4)
int lastTurn = turn;                           ///< Previous player's turn
std::vector<int> nextTurn;                     ///< Queue of upcoming player turns
std::vector<int> winners;                      ///< List of players who have completed the game

//------------------------------------------------------------------------------
// Anonymous Namespace for Internal Constants and Helpers
//------------------------------------------------------------------------------
namespace {
    /// Game configuration constants
    constexpr int MAX_PLAYERS = 4;              ///< Maximum number of players
    constexpr int MAX_TOKENS = 4;               ///< Maximum tokens per player
    constexpr int BOARD_SIZE = 4;               ///< Size of each grid quadrant
    constexpr int MAX_DICE_VALUE = 6;           ///< Maximum value on dice
    
    /**
     * @brief Gets reference to the global random number generator
     * @return Reference to mt19937 random number generator
     */
    std::mt19937& getRNG() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }
    
    /**
     * @brief Checks if a value falls within an inclusive range
     * @param value Value to check
     * @param min Minimum allowed value
     * @param max Maximum allowed value
     * @return true if value is within range, false otherwise
     */
    bool isInRange(int value, int min, int max) {
        return value >= min && value <= max;
    }
}

//------------------------------------------------------------------------------
// Grid Position Validation Functions
//------------------------------------------------------------------------------

/**
 * @brief Validates a 3D grid position
 * @param pos Tuple containing (grid, row, col) coordinates
 * @return true if position is valid, false otherwise
 */
bool isValidPosition(const GridPosition& pos) {
    auto [grid, row, col] = pos;
    return isValidGridIndex(grid) && isValidRowCol(row, col);
}

/**
 * @brief Validates a grid index
 * @param gridIndex Index of grid quadrant to validate
 * @return true if index is valid, false otherwise
 */
bool isValidGridIndex(int gridIndex) {
    return isInRange(gridIndex, 0, MAX_PLAYERS - 1);
}

/**
 * @brief Validates row and column coordinates
 * @param row Row coordinate to validate
 * @param col Column coordinate to validate
 * @return true if coordinates are valid, false otherwise
 */
bool isValidRowCol(int row, int col) {
    return isInRange(row, 0, BOARD_SIZE - 1) && isInRange(col, 0, BOARD_SIZE - 1);
}

//------------------------------------------------------------------------------
// Token Movement and Safety Functions
//------------------------------------------------------------------------------

/**
 * @brief Determines if a token position is safe from capture
 * @param pos Position to check
 * @return true if position is safe, false otherwise
 */
bool isTokenSafe(const GridPosition& pos) {
    if (!isValidPosition(pos)) return false;
    auto [grid, row, col] = pos;
    return (row == 2 && col == 1) || (row == 0 && col == 3);
}

/**
 * @brief Checks if a token can move between two positions
 * @param from Starting position
 * @param to Destination position
 * @return true if movement is legal, false otherwise
 */
bool canTokenMove(const GridPosition& from, const GridPosition& to) {
    return isValidPosition(from) && isValidPosition(to) && isPathClear(from, to);
}

/**
 * @brief Checks if a position is a player's home position
 * @param pos Position to check
 * @param playerID ID of player (0-3)
 * @return true if position is player's home, false otherwise
 */
bool isHomePosition(const GridPosition& pos, int playerID) {
    if (!isValidPosition(pos) || !isValidGridIndex(playerID)) return false;
    auto [grid, row, col] = pos;
    return grid == playerID && row == 3 && col == 3;
}

/**
 * @brief Checks if a position is a player's starting position
 * @param pos Position to check
 * @param playerID ID of player (0-3)
 * @return true if position is player's start, false otherwise
 */
bool isStartPosition(const GridPosition& pos, int playerID) {
    if (!isValidPosition(pos) || !isValidGridIndex(playerID)) return false;
    auto [grid, row, col] = pos;
    return grid == playerID && row == 0 && col == 0;
}

//------------------------------------------------------------------------------
// Turn Management Functions
//------------------------------------------------------------------------------

/**
 * @brief Generates a random turn order for players
 * Clears and repopulates nextTurn vector with shuffled player IDs
 */
void GeneratePlayerTurns() {
    nextTurn.clear();
    nextTurn.resize(MAX_PLAYERS);
    std::iota(nextTurn.begin(), nextTurn.end(), 1);
    std::shuffle(nextTurn.begin(), nextTurn.end(), getRNG());
}

/**
 * @brief Gets the ID of the next player to take their turn
 * @return Player ID of next active player
 */
int getTurn() {
    if (nextTurn.empty()) {
        GeneratePlayerTurns();
    }
    
    while (!nextTurn.empty()) {
        int nextPlayer = nextTurn.back();
        nextTurn.pop_back();
        
        if (!isPlayerWinner(nextPlayer)) {
            return nextPlayer;
        }
    }
    
    return 1;  // Default to player 1 if no valid turns
}

/**
 * @brief Checks if a player has won the game
 * @param playerID ID of player to check
 * @return true if player has won, false otherwise
 */
bool isPlayerWinner(int playerID) {
    return std::find(winners.begin(), winners.end(), playerID) != winners.end();
}

/**
 * @brief Adds a player to the winners list
 * @param playerID ID of winning player
 */
void addWinner(int playerID) {
    if (!isPlayerWinner(playerID)) {
        winners.push_back(playerID);
    }
}

/**
 * @brief Checks if the game is complete
 * @return true if enough players have won, false otherwise
 */
bool isGameComplete() {
    return winners.size() >= MAX_PLAYERS - 1;
}

//------------------------------------------------------------------------------
// Grid Management Functions
//------------------------------------------------------------------------------

/**
 * @brief Initializes the game board grid
 * @return true if initialization successful, false otherwise
 * @throws std::invalid_argument if numTokens is invalid
 */
bool initializeGridBoard() {
    try {
        if (numTokens <= 0 || numTokens > MAX_TOKENS) {
            throw std::invalid_argument("Invalid number of tokens");
        }

        cleanupGridBoard();  // Clean up existing grid if any

        LudoGrid = new GridPosition*[MAX_PLAYERS];
        for (int i = 0; i < MAX_PLAYERS; i++) {
            LudoGrid[i] = new GridPosition[numTokens];
            for (int j = 0; j < numTokens; j++) {
                LudoGrid[i][j] = std::make_tuple(-1, -1, -1);
            }
        }
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Grid initialization error: " << e.what() << std::endl;
        cleanupGridBoard();
        return false;
    }
}

/**
 * @brief Deallocates grid memory
 */
void cleanupGridBoard() {
    if (LudoGrid) {
        for (int i = 0; i < MAX_PLAYERS; i++) {
            delete[] LudoGrid[i];
        }
        delete[] LudoGrid;
        LudoGrid = nullptr;
    }
}

/**
 * @brief Gets the position of a specific token
 * @param playerID Owner of token
 * @param tokenID ID of token to locate
 * @return Position tuple, (-1,-1,-1) if invalid
 */
GridPosition getGridPosition(int playerID, int tokenID) {
    if (isValidGridIndex(playerID) && isInRange(tokenID, 0, numTokens - 1)) {
        return LudoGrid[playerID][tokenID];
    }
    return std::make_tuple(-1, -1, -1);
}

/**
 * @brief Updates the position of a specific token
 * @param playerID Owner of token
 * @param tokenID ID of token to update
 * @param newPos New position for token
 */
void updateGridPosition(int playerID, int tokenID, const GridPosition& newPos) {
    if (isValidGridIndex(playerID) && isInRange(tokenID, 0, numTokens - 1)) {
        LudoGrid[playerID][tokenID] = newPos;
    }
}

//------------------------------------------------------------------------------
// Dice Management Functions
//------------------------------------------------------------------------------

/**
 * @brief Resets all dice-related values to initial state
 */
void resetDiceValue() {
    diceVal.clear();
    diceVal.resize(3, 0);
    diceCount = 0;
    dice = 1;
    moveDice = true;
}

/**
 * @brief Records a new dice roll value
 * @param value Value rolled on dice
 */
void newDiceNumber(int value) {
    if (isValidDiceRoll(value)) {
        dice = value;
        if (diceCount < 3) {
            diceVal[diceCount++] = value;
        }
    }
}

/**
 * @brief Validates a dice roll value
 * @param value Value to validate
 * @return true if value is valid, false otherwise
 */
bool isValidDiceRoll(int value) {
    return isInRange(value, 1, MAX_DICE_VALUE);
}

/**
 * @brief Calculates sum of all dice values
 * @return Total of all dice values
 */
int getCurrentDiceSum() {
    return std::accumulate(diceVal.begin(), diceVal.end(), 0);
}

/**
 * @brief Checks if dice can be rolled
 * @return true if dice roll allowed, false otherwise
 */
bool canRollDice() {
    return moveDice && diceCount < 3;
}

//------------------------------------------------------------------------------
// Game State Management Functions
//------------------------------------------------------------------------------

/**
 * @brief Resets all game state variables to initial values
 */
void resetcompleteGame() {
    numTokens = -1;
    cleanupGridBoard();
    resetDiceValue();
    movePlayer = false;
    moveDice = true;
    turn = 1;
    lastTurn = turn;
    nextTurn.clear();
    winners.clear();
}

//------------------------------------------------------------------------------
// Position Calculation Functions
//------------------------------------------------------------------------------

/**
 * @brief Calculates next position after moving specified steps
 * @param current Starting position
 * @param steps Number of steps to move
 * @return New position after movement
 */
GridPosition calculateNextPosition(const GridPosition& current, int steps) {
    if (!isValidPosition(current) || steps <= 0) {
        return current;
    }
    
    auto [grid, row, col] = current;
    
    // Handle movement based on grid position
    if (grid == 0) { // Top grid
        if (col + steps >= BOARD_SIZE) {
            // Move to next grid when reaching edge
            return std::make_tuple(1, 0, steps - (BOARD_SIZE - col));
        }
        return std::make_tuple(grid, row, col + steps);
    }
    else if (grid == 1) { // Right grid 
        if (row + steps >= BOARD_SIZE) {
            return std::make_tuple(2, 0, steps - (BOARD_SIZE - row));
        }
        return std::make_tuple(grid, row + steps, col);
    }
    else if (grid == 2) { // Bottom grid
        if (col - steps < 0) {
            return std::make_tuple(3, BOARD_SIZE-1, (steps - col) - 1);
        }
        return std::make_tuple(grid, row, col - steps);
    }
    else { // Left grid
        if (row - steps < 0) {
            return std::make_tuple(0, BOARD_SIZE-1, steps - row - 1); 
        }
        return std::make_tuple(grid, row - steps, col);
    }
}

/**
 * @brief Checks if path between positions is clear of obstacles
 * @param from Starting position
 * @param to Destination position
 * @return true if path is clear, false if blocked
 */
bool isPathClear(const GridPosition& from, const GridPosition& to) {
    if (!isValidPosition(from) || !isValidPosition(to)) {
        return false;
    }
    
    // Check if positions are in same grid
    auto [fromGrid, fromRow, fromCol] = from;
    auto [toGrid, toRow, toCol] = to;

    // Get intermediate positions between from and to
    std::vector<GridPosition> path;
    int steps = 1;
    GridPosition current = from;
    GridPosition next;

    // Build path of positions
    while (current != to) {
        next = calculateNextPosition(current, 1);
        if (next == current) { // No valid next position
            return false;
        }
        path.push_back(next);
        current = next;
        if (steps++ > BOARD_SIZE * 4) { // Prevent infinite loop
            return false;
        }
    }

    // Check if any position in path is occupied
    for (const auto& pos : path) {
        auto [grid, row, col] = pos;
        if (getGridPosition(grid, row * BOARD_SIZE + col) != std::make_tuple(-1, -1, -1)) {
            return false; // Path is blocked
        }
    }

    return true;
}

/**
 * @brief Calculates distance between two positions
 * @param from Starting position
 * @param to Destination position
 * @return Number of steps between positions, -1 if invalid path
 */
int calculateDistance(const GridPosition& from, const GridPosition& to) {
    if (!isValidPosition(from) || !isValidPosition(to)) {
        return -1;
    }
    
    auto [fromGrid, fromRow, fromCol] = from;
    auto [toGrid, toRow, toCol] = to;
    // Calculate distance by following the path between positions
    int distance = 0;
    GridPosition current = from;
    
    // Follow path until we reach destination or detect invalid path
    while (current != to) {
        GridPosition next = calculateNextPosition(current, 1);
        if (next == current) { // No valid next position found
            return -1;
        }
        distance++;
        current = next;
        
        // Prevent infinite loops
        if (distance > BOARD_SIZE * 4) {
            return -1;
        }
    }
    
    return distance;
}