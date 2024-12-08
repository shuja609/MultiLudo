#include "../include/Utils.h"
#include <algorithm>
#include <random>
#include <chrono>
#include <stdexcept>
#include <iostream>
// Global variables (kept for compatibility)
int numTokens = -1;
std::tuple<int, int, int> **LudoGrid;
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
    // Random number generation utilities
    std::mt19937& getRNG() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }

    int getRandomPlayer() {
        std::uniform_int_distribution<> dist(1, 4);
        return dist(getRNG());
    }

    // Grid position validation
    bool isValidGridPosition(int grid, int row, int col) {
        return grid >= 0 && grid < 4 && 
               row >= 0 && row < 4 && 
               col >= 0 && col < 4;
    }

    // Safe position checks
    bool isSafePosition(int grid, int row, int col) {
        return (row == 2 && col == 1) || (row == 0 && col == 3);
    }
}

bool isTokenSafe(std::tuple<int, int, int> g) {
    try {
        auto [grid, row, col] = g;
        
        if (!isValidGridPosition(grid, row, col)) {
            return false;
        }

        return isSafePosition(grid, row, col);
    }
    catch (const std::exception& e) {
        std::cerr << "Error checking token safety: " << e.what() << std::endl;
        return false;
    }
}

void GenerateTurns() {
    try {
        nextTurn.clear();
        nextTurn.resize(4);
        
        std::vector<int> players{1, 2, 3, 4};
        std::shuffle(players.begin(), players.end(), getRNG());
        
        // Fill nextTurn with shuffled player numbers
        std::copy(players.begin(), players.end(), nextTurn.begin());
    }
    catch (const std::exception& e) {
        std::cerr << "Error generating turns: " << e.what() << std::endl;
        // Fallback to simple sequential order
        nextTurn = {1, 2, 3, 4};
    }
}

int getTurn() {
    try {
        if (nextTurn.empty()) {
            GenerateTurns();
        }

        // Get next available player
        while (!nextTurn.empty()) {
            int nextPlayer = nextTurn.back();
            nextTurn.pop_back();

            // Skip if player is already a winner
            if (std::find(winners.begin(), winners.end(), nextPlayer) == winners.end()) {
                return nextPlayer;
            }

            // If we've used all turns, regenerate
            if (nextTurn.empty()) {
                GenerateTurns();
            }
        }

        // Fallback: return first non-winner player
        for (int i = 1; i <= 4; ++i) {
            if (std::find(winners.begin(), winners.end(), i) == winners.end()) {
                return i;
            }
        }

        return 1;  // Default fallback
    }
    catch (const std::exception& e) {
        std::cerr << "Error getting turn: " << e.what() << std::endl;
        return 1;  // Default fallback
    }
}

// Grid management functions
bool initializeGrid() {
    try {
        if (numTokens <= 0 || numTokens > 4) {
            throw std::invalid_argument("Invalid number of tokens");
        }

        LudoGrid = new std::tuple<int, int, int>*[4];
        for (int i = 0; i < 4; i++) {
            LudoGrid[i] = new std::tuple<int, int, int>[numTokens];
            for (int j = 0; j < numTokens; j++) {
                LudoGrid[i][j] = std::make_tuple(-1, -1, -1);
            }
        }
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing grid: " << e.what() << std::endl;
        cleanupGrid();
        return false;
    }
}

void cleanupGrid() {
    if (LudoGrid != nullptr) {
        for (int i = 0; i < 4; i++) {
            delete[] LudoGrid[i];
        }
        delete[] LudoGrid;
        LudoGrid = nullptr;
    }
}

// Dice management functions
void resetDice() {
    diceVal.clear();
    diceVal.resize(3, 0);
    diceCount = 0;
    dice = 1;
}

void updateDiceValue(int value) {
    if (value >= 1 && value <= 6) {
        dice = value;
        if (diceCount < 3) {
            diceVal[diceCount++] = value;
        }
    }
}

// Game state management
void resetGameState() {
    movePlayer = false;
    moveDice = true;
    turn = 1;
    lastTurn = turn;
    nextTurn.clear();
    winners.clear();
    resetDice();
} 