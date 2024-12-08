#include "../include/Token.h"
#include "../include/Utils.h"
#include <iostream>
#include <stdexcept>

/**
 * @brief Default constructor initializes token state
 */
Token::Token() {
    sem_init(&semToken, 0, 0);
    isSafe = true;
    finished = false;
    gridPos = std::make_tuple(-1, -1, -1);
    isOut = false;
    canGoHome = false;
    id = -1;
    x = y = initX = initY = 0;
}

/**
 * @brief Set token's texture and reset its state
 * @param t Texture to use for the token
 */
void Token::setTexture(Texture2D t) {
    sem_init(&semToken, 0, 0);
    isSafe = true;
    gridPos = std::make_tuple(-1, -1, -1);
    isOut = false;
    canGoHome = false;
    finished = false;
    id = -1;
    x = y = 0;
    token = t;
}

/**
 * @brief Set token's starting position based on player ID
 * @param i Player/Token ID (0-3)
 */
void Token::setStart(int i) {
    id = i;
    switch (id) {
        case 0: x = 60;  y = 360; break;
        case 1: x = 480; y = 60;  break;
        case 2: x = 780; y = 480; break;
        case 3: x = 360; y = 780; break;
        default:
            throw std::invalid_argument("Invalid player ID");
    }
    initX = x;
    initY = y;
}

/**
 * @brief Update token's grid position and check safety
 */
void Token::updateGrid() {
    if (!isValidGridPosition(gridPos)) {
        throw std::runtime_error("Invalid grid position");
    }
    
    LudoGrid[id][gridID] = gridPos;
    isSafe = isTokenSafe(gridPos);
}

/**
 * @brief Draw token at its current or initial position
 */
void Token::drawInit() {
    int drawX = isOut && !finished ? x : initX;
    int drawY = isOut && !finished ? y : initY;
    DrawTexture(token, drawX, drawY, WHITE);
}

/**
 * @brief Move token out of home onto the board
 */
void Token::outToken() {
    sem_post(&semToken);
    isOut = true;
    gridPos = std::make_tuple(id, 2, 1);
    updateGrid();
    DrawTexture(token, x, y, WHITE);
}

/**
 * @brief Move token back to home position
 */
void Token::inToken() {
    sem_wait(&semToken);
    isOut = false;
    setStart(id);
    gridPos = std::make_tuple(-1, -1, -1);
}

/**
 * @brief Move token by specified number of steps
 * @param roll Number of steps to move
 */
void Token::move(int roll) {
    if (roll == 0) return;
    validateMove(roll);

    if (canGoHome && id == std::get<0>(gridPos)) {
        handleHomeStretch(roll);
    } else {
        handleNormalPath(roll);
    }
}

/**
 * @brief Check if token can move to the specified position
 */
bool Token::canMoveToPosition(const std::tuple<int, int, int>& newPos) const {
    // Check if position is valid
    if (!isValidGridPosition(newPos)) return false;

    // Check if position is occupied by another token
    for (int i = 0; i < 4; i++) {
        if (i == id) continue;
        for (int j = 0; j < numTokens; j++) {
            if (LudoGrid[i][j] == newPos && isTokenSafe(newPos)) {
                return false;
            }
        }
    }

    return true;
}

/**
 * @brief Update token's screen position
 */
void Token::updatePosition(int newX, int newY) {
    x = newX;
    y = newY;
}

/**
 * @brief Handle movement in the home stretch
 */
void Token::handleHomeStretch(int roll) {
    auto [g, r, c] = gridPos;
    if (c + roll <= 6) {
        x += roll * 60;
        std::get<2>(gridPos) = c + roll;
        if (std::get<2>(gridPos) == 6) {
            sem_destroy(&semToken);
            isOut = false;
            finished = true;
        }
    }
}

/**
 * @brief Handle movement on the normal path
 */
void Token::handleNormalPath(int roll) {
    auto [g, r, c] = gridPos;
    int next = 0, cur = 0;

    if (c + roll >= 5) {
        next = (c + roll) - 5;
        cur = roll - next;
    } else {
        cur = roll;
    }

    // Update position based on current grid section
    switch (g) {
        case 0:
            handleGrid0Movement(r, c, cur, next);
            break;
        case 1:
            handleGrid1Movement(r, c, cur, next);
            break;
        case 2:
            handleGrid2Movement(r, c, cur, next);
            break;
        case 3:
            handleGrid3Movement(r, c, cur, next);
            break;
    }
}

/**
 * @brief Validate if a grid position is valid
 */
bool Token::isValidGridPosition(const std::tuple<int, int, int>& pos) const {
    auto [g, r, c] = pos;
    if (g == -1 && r == -1 && c == -1) return true; // Home position
    return g >= 0 && g < 4 && r >= 0 && r < 3 && c >= 0 && c < 6;
}

/**
 * @brief Validate if a move is legal
 */
void Token::validateMove(int roll) const {
    if (roll < 0 || roll > 6) {
        throw std::invalid_argument("Invalid dice roll");
    }
    if (!isOut && roll != 6) {
        throw std::runtime_error("Token must be out to move");
    }
}

// Private helper functions for grid-specific movement
void Token::handleGrid0Movement(int r, int c, int cur, int next) {
    switch (r) {
        case 0:
            x -= cur * 60;
            std::get<2>(gridPos) = c + cur;
            if (next >= 1) {
                y -= 60;
                std::get<1>(gridPos) = 1;
                std::get<2>(gridPos) = 0;
                move(next - 1);
            }
            break;
        case 1:
            y -= cur * 60;
            std::get<2>(gridPos) = c + cur;
            if (next >= 1) {
                std::get<0>(gridPos) = 1;
                std::get<1>(gridPos) = 2;
                std::get<2>(gridPos) = 0;
                move(next - 1);
            }
            break;
        case 2:
            if (c == 0) canGoHome = true;
            x += cur * 60;
            std::get<2>(gridPos) = c + cur;
            if (next >= 1) {
                std::get<0>(gridPos) = 1;
                std::get<1>(gridPos) = 2;
                std::get<2>(gridPos) = 0;
                move(next - 1);
            }
            break;
    }
}

void Token::handleGrid1Movement(int r, int c, int cur, int next) {
    switch (r) {
        case 0:
            y -= cur * 60;
            std::get<2>(gridPos) = c + cur;
            if (next >= 1) {
                x += 60;
                std::get<1>(gridPos) = 1;
                std::get<2>(gridPos) = 0;
                move(next - 1);
            }
            break;
        case 1:
            if (canGoHome && id == std::get<0>(gridPos)) {
                if (c + next <= 6) {
                    y += next * 60;
                    std::get<2>(gridPos) = c + next;
                    if (std::get<2>(gridPos) == 6) {
                        sem_destroy(&semToken);
                        finished = true;
                        isOut = false;
                    }
                }
            }
            break;
        case 2:
            y += cur * 60;
            std::get<2>(gridPos) = c + cur;
            if (next >= 1) {
                std::get<0>(gridPos) = 2;
                std::get<1>(gridPos) = 0;
                std::get<2>(gridPos) = next - 1;
            }
            break;
    }
}

void Token::handleGrid2Movement(int r, int c, int cur, int next) {
    switch (r) {
        case 0:
            x += cur * 60;
            std::get<2>(gridPos) = c + cur;
            if (next >= 1) {
                y += 60;
                std::get<1>(gridPos) = 1;
                std::get<2>(gridPos) = 0;
                move(next - 1);
            }
            break;
        case 1:
            if (canGoHome && id == std::get<0>(gridPos)) {
                if (c + next <= 6) {
                    x -= next * 60;
                    std::get<2>(gridPos) = c + next;
                    if (std::get<2>(gridPos) == 6) {
                        sem_destroy(&semToken);
                        finished = true;
                        isOut = false;
                    }
                }
            }
            break;
        case 2:
            x -= cur * 60;
            std::get<2>(gridPos) = c + cur;
            if (next >= 1) {
                std::get<0>(gridPos) = 3;
                std::get<1>(gridPos) = 0;
                std::get<2>(gridPos) = next - 1;
            }
            break;
    }
}

void Token::handleGrid3Movement(int r, int c, int cur, int next) {
    switch (r) {
        case 0:
            y += cur * 60;
            std::get<2>(gridPos) = c + cur;
            if (next >= 1) {
                x -= 60;
                std::get<1>(gridPos) = 1;
                std::get<2>(gridPos) = 0;
                move(next - 1);
            }
            break;
        case 1:
            if (canGoHome && id == std::get<0>(gridPos)) {
                if (c + next <= 6) {
                    y -= next * 60;
                    std::get<2>(gridPos) = c + next;
                    if (std::get<2>(gridPos) == 6) {
                        sem_destroy(&semToken);
                        finished = true;
                        isOut = false;
                    }
                }
            }
            break;
        case 2:
            y -= cur * 60;
            std::get<2>(gridPos) = c + cur;
            if (next >= 1) {
                std::get<0>(gridPos) = 0;
                std::get<1>(gridPos) = 0;
                std::get<2>(gridPos) = next - 1;
            }
            break;
    }
}