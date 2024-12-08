


// - **Shuja Uddin** (22i-2553) | SE-D | FAST NUCES Islamabad
// - **Amna Hassan** (22i-8759) | SE-D | FAST NUCES Islamabad  
// - **Samra Saleem** (22i-2727) | SE-D | FAST NUCES Islamabad


/**
 * @file Token.cpp
 * @brief Implementation of the Token class for the Ludo game
 * @details Contains the implementation of token movement, positioning, and game logic
 */

#include "../include/Token.h"
#include "../include/Utils.h"
#include <iostream>
#include <stdexcept>

/**
 * @brief Default constructor initializes token state
 * @details Initializes semaphore, position coordinates, and state flags to default values
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
 * @brief Sets the token's texture and resets its state
 * @param t The texture to be applied to the token
 * @details Reinitializes all token properties and assigns the provided texture
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
 * @brief Sets the token's starting position based on player ID
 * @param i Player/Token ID (0-3)
 * @throws std::invalid_argument if player ID is invalid
 * @details Maps player IDs to specific board coordinates for initial positioning
 */
void Token::setStart(int i) {
    id = i;
    switch (id) {
        case 0: x = 60;  y = 360; break;  // Top player starting position
        case 1: x = 480; y = 60;  break;  // Right player starting position
        case 2: x = 780; y = 480; break;  // Bottom player starting position
        case 3: x = 360; y = 780; break;  // Left player starting position
        default:
            throw std::invalid_argument("Invalid player ID");
    }
    initX = x;
    initY = y;
}

/**
 * @brief Updates token's grid position and checks safety status
 * @throws std::runtime_error if grid position is invalid
 * @details Updates the global game grid with token's position and verifies safety status
 */
void Token::updateGrid() {
    if (!isValidGridPosition(gridPos)) {
        throw std::runtime_error("Invalid grid position");
    }
    
    LudoGrid[id][gridID] = gridPos;
    isSafe = isTokenSafe(gridPos);
}

/**
 * @brief Renders token at its current or initial position
 * @details Draws the token texture at appropriate coordinates based on game state
 */
void Token::drawInit() {
    int drawX = isOut && !finished ? x : initX;
    int drawY = isOut && !finished ? y : initY;
    DrawTexture(token, drawX, drawY, WHITE);
}

/**
 * @brief Moves token from home position onto the game board
 * @details Updates token state, position, and grid coordinates when leaving home
 */
void Token::outToken() {
    sem_post(&semToken);
    isOut = true;
    gridPos = std::make_tuple(id, 2, 1);
    updateGrid();
    DrawTexture(token, x, y, WHITE);
}

/**
 * @brief Returns token to home position
 * @details Resets token state and position to initial values
 */
void Token::inToken() {
    sem_wait(&semToken);
    isOut = false;
    setStart(id);
    gridPos = std::make_tuple(-1, -1, -1);
}

/**
 * @brief Moves token by specified number of steps
 * @param roll Number of steps to move
 * @details Handles both normal path movement and home stretch movement based on token state
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
 * @brief Checks if token can move to specified position
 * @param newPos Target position tuple (grid, row, column)
 * @return bool True if move is valid, false otherwise
 * @details Validates position and checks for token collisions
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
 * @brief Updates token's screen coordinates
 * @param newX New x-coordinate
 * @param newY New y-coordinate
 */
void Token::updatePosition(int newX, int newY) {
    x = newX;
    y = newY;
}

/**
 * @brief Handles token movement in the home stretch
 * @param roll Number of steps to move
 * @details Manages final approach to home position and completion state
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
 * @brief Handles token movement on the normal path
 * @param roll Number of steps to move
 * @details Calculates movement across grid sections and manages transitions
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
 * @brief Validates if a grid position is valid
 * @param pos Position tuple to validate
 * @return bool True if position is valid, false otherwise
 */
bool Token::isValidGridPosition(const std::tuple<int, int, int>& pos) const {
    auto [g, r, c] = pos;
    if (g == -1 && r == -1 && c == -1) return true; // Home position
    return g >= 0 && g < 4 && r >= 0 && r < 3 && c >= 0 && c < 6;
}

/**
 * @brief Validates if a move is legal
 * @param roll Dice roll value to validate
 * @throws std::invalid_argument if dice roll is invalid
 * @throws std::runtime_error if token movement is illegal
 */
void Token::validateMove(int roll) const {
    if (roll < 0 || roll > 6) {
        throw std::invalid_argument("Invalid dice roll");
    }
    if (!isOut && roll != 6) {
        throw std::runtime_error("Token must be out to move");
    }
}

/**
 * @brief Handles movement in grid section 0
 * @param r Current row
 * @param c Current column
 * @param cur Current movement steps
 * @param next Remaining movement steps
 * @details Manages token movement and transitions in the top grid section
 */
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

/**
 * @brief Handles movement in grid section 1
 * @param r Current row
 * @param c Current column
 * @param cur Current movement steps
 * @param next Remaining movement steps
 * @details Manages token movement and transitions in the right grid section
 */
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

/**
 * @brief Handles movement in grid section 2
 * @param r Current row
 * @param c Current column
 * @param cur Current movement steps
 * @param next Remaining movement steps
 * @details Manages token movement and transitions in the bottom grid section
 */
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

/**
 * @brief Handles movement in grid section 3
 * @param r Current row
 * @param c Current column
 * @param cur Current movement steps
 * @param next Remaining movement steps
 * @details Manages token movement and transitions in the left grid section
 */
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