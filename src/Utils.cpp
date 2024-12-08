#include "../include/Utils.h"
#include <algorithm>
#include <random>

// Global variables
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

bool isTokenSafe(std::tuple<int, int, int> g) {
    for (int i = 0; i < 4; i++) {
        if (g == std::make_tuple(i, 2, 1) || g == std::make_tuple(i, 0, 3))
            return true;
    }
    return false;
}

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