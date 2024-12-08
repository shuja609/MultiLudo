#include "../include/Utils.h"
#include <algorithm>
#include <random>
#include <stdexcept>
#include <iostream>
#include <numeric>

// Game state variables initialization
int numTokens = -1;
std::tuple<int, int, int> **LudoGrid = nullptr;
std::vector<int> diceVal(3, 0);
int diceCount = 0;
int dice = 1;
bool movePlayer = false;
bool moveDice = true;
int turn = 1;
int lastTurn = turn;
std::vector<int> nextTurn;
std::vector<int> winners;

namespace {
    // Constants
    constexpr int MAX_PLAYERS = 4;
    constexpr int MAX_TOKENS = 4;
    constexpr int BOARD_SIZE = 4;
    constexpr int MAX_DICE_VALUE = 6;
    
    // Random number generation
    std::mt19937& getRNG() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }
    
    // Internal validation helpers
    bool isInRange(int value, int min, int max) {
        return value >= min && value <= max;
    }
}

// Grid position validation
bool isValidPosition(const GridPosition& pos) {
    auto [grid, row, col] = pos;
    return isValidGridIndex(grid) && isValidRowCol(row, col);
}

bool isValidGridIndex(int gridIndex) {
    return isInRange(gridIndex, 0, MAX_PLAYERS - 1);
}

bool isValidRowCol(int row, int col) {
    return isInRange(row, 0, BOARD_SIZE - 1) && isInRange(col, 0, BOARD_SIZE - 1);
}

// Token movement and safety
bool isTokenSafe(const GridPosition& pos) {
    if (!isValidPosition(pos)) return false;
    auto [grid, row, col] = pos;
    return (row == 2 && col == 1) || (row == 0 && col == 3);
}

bool canTokenMove(const GridPosition& from, const GridPosition& to) {
    return isValidPosition(from) && isValidPosition(to) && isPathClear(from, to);
}

bool isHomePosition(const GridPosition& pos, int playerID) {
    if (!isValidPosition(pos) || !isValidGridIndex(playerID)) return false;
    auto [grid, row, col] = pos;
    return grid == playerID && row == 3 && col == 3;
}

bool isStartPosition(const GridPosition& pos, int playerID) {
    if (!isValidPosition(pos) || !isValidGridIndex(playerID)) return false;
    auto [grid, row, col] = pos;
    return grid == playerID && row == 0 && col == 0;
}

// Turn management
void GeneratePlayerTurns() {
    nextTurn.clear();
    nextTurn.resize(MAX_PLAYERS);
    std::iota(nextTurn.begin(), nextTurn.end(), 1);
    std::shuffle(nextTurn.begin(), nextTurn.end(), getRNG());
}

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

bool isPlayerWinner(int playerID) {
    return std::find(winners.begin(), winners.end(), playerID) != winners.end();
}

void addWinner(int playerID) {
    if (!isPlayerWinner(playerID)) {
        winners.push_back(playerID);
    }
}

bool isGameComplete() {
    return winners.size() >= MAX_PLAYERS - 1;
}

// Grid management
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

void cleanupGridBoard() {
    if (LudoGrid) {
        for (int i = 0; i < MAX_PLAYERS; i++) {
            delete[] LudoGrid[i];
        }
        delete[] LudoGrid;
        LudoGrid = nullptr;
    }
}

GridPosition getGridPosition(int playerID, int tokenID) {
    if (isValidGridIndex(playerID) && isInRange(tokenID, 0, numTokens - 1)) {
        return LudoGrid[playerID][tokenID];
    }
    return std::make_tuple(-1, -1, -1);
}

void updateGridPosition(int playerID, int tokenID, const GridPosition& newPos) {
    if (isValidGridIndex(playerID) && isInRange(tokenID, 0, numTokens - 1)) {
        LudoGrid[playerID][tokenID] = newPos;
    }
}

// Dice management
void resetDiceValue() {
    diceVal.clear();
    diceVal.resize(3, 0);
    diceCount = 0;
    dice = 1;
    moveDice = true;
}

void newDiceNumber(int value) {
    if (isValidDiceRoll(value)) {
        dice = value;
        if (diceCount < 3) {
            diceVal[diceCount++] = value;
        }
    }
}

bool isValidDiceRoll(int value) {
    return isInRange(value, 1, MAX_DICE_VALUE);
}

int getCurrentDiceSum() {
    return std::accumulate(diceVal.begin(), diceVal.end(), 0);
}

bool canRollDice() {
    return moveDice && diceCount < 3;
}

// Game state management
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

// Position calculations
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