#include "../include/Token.h"
#include "../include/Utils.h"
#include <iostream>

/**
 * @brief Default constructor for Token class
 * Initializes a token with default values and creates a semaphore
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
 * @param t The texture to apply to this token
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
 * @param i The player ID (0-3) determining start position
 */
void Token::setStart(int i) {
    id = i;
    switch (id) {
        case 0:  // Left player
            x = 60;
            y = 360;
            break;
        case 1:  // Top player
            x = 480;
            y = 60;
            break;
        case 2:  // Right player
            x = 780;
            y = 480;
            break;
        case 3:  // Bottom player
            x = 360;
            y = 780;
            break;
        default:
            break;
    }
}

/**
 * @brief Updates the token's grid position and safety status
 * Updates the global LudoGrid with token's current position and checks if position is safe
 */
void Token::updateGrid() {
    LudoGrid[id][gridID] = gridPos;
    if (isTokenSafe(gridPos))
        isSafe = true;
    else
        isSafe = false;
}

/**
 * @brief Draws the token at its current or initial position
 * Renders token texture based on whether it's in play or at home
 */
void Token::drawInit() {
    if (isOut == false && !finished)
        DrawTexture(token, initX, initY, WHITE);
    else {
        DrawTexture(token, x, y, WHITE);
    }
}

/**
 * @brief Moves token out of starting area into play
 * Signals semaphore and updates token position for game start
 */
void Token::outToken() {
    sem_post(&semToken);
    isOut = true;
    gridPos = std::make_tuple(id, 2, 1);
    DrawTexture(token, x, y, WHITE);
}

/**
 * @brief Returns token to starting position
 * Waits on semaphore and resets token state to initial position
 */
void Token::inToken() {
    sem_wait(&semToken);
    isOut = false;
    setStart(id);
    gridPos = std::make_tuple(-1, -1, -1);
}

/**
 * @brief Moves the token based on dice roll
 * Handles complex movement logic including:
 * - Path following around board
 * - Entering home stretch
 * - Reaching final position
 * - Coordinate and grid position updates
 * 
 * @param roll Number of spaces to move from dice roll
 */
void Token::move(int roll) {
    if (roll == 0)
        return;
    std::cout << "\nDice Val " << roll << std::endl;
    
    // Extract current grid position components
    int g = std::get<0>(gridPos);  // Grid quadrant (0-3)
    int r = std::get<1>(gridPos);  // Row position
    int c = std::get<2>(gridPos);  // Column position
    
    std::cout << "Grid: " << g
         << " Row: " << r
         << " Col: " << c << std::endl;
         
    // Calculate movement overflow
    int next = 0, cur = 0;
    std::cout << "C+Roll " << c + roll << std::endl;
    if (c + roll >= 5) {
        next = (c + roll) - 5;  // Spaces to move after current segment
        cur = roll - next;      // Spaces to move in current segment
        std::cout << "Next: " << next
             << " Cur: " << cur << std::endl;
    }
    else {
        cur = roll;
    }

    // Handle movement based on current grid quadrant and row
    switch (g) {
        case 0:  // Left quadrant
            switch (r) {
                case 0:  // Top row
                    x = x - (cur * 60);
                    std::get<2>(gridPos) = c + cur;
                    if (next >= 1) {
                        y = y - 60;
                        next--;
                        std::get<1>(gridPos) = 1;
                        std::get<2>(gridPos) = 0;
                        move(next);
                    }
                    break;
                case 1:  // Home stretch
                    if (canGoHome && id == std::get<0>(gridPos)) {
                        if (roll + c <= 6) {
                            x = x + (roll * 60);
                            std::get<2>(gridPos) = c + roll;
                            if (std::get<2>(gridPos) == 6) {
                                sem_destroy(&semToken);
                                isOut = false;
                                finished = true;
                                x = -100;
                                y = -100;
                            }
                        }
                    }
                    else {
                        next = 1;
                        y = y - (next * 60);
                        cur--;
                        std::get<1>(gridPos) = 2;
                        std::get<2>(gridPos) = 0;
                        move(cur);
                        return;
                    }
                    break;
                case 2:  // Bottom row
                    x = x + (cur * 60);
                    std::get<2>(gridPos) = c + cur;
                    if (next != 0) {
                        x += 60;
                        y = y - (next * 60);
                        std::get<0>(gridPos) = 1;
                        std::get<1>(gridPos) = 0;
                        std::get<2>(gridPos) = next - 1;
                    }
                    break;
                default:
                    break;
            }
            break;
        case 1:  // Top quadrant
            switch (r) {
                case 0:  // Left row
                    y = y - (cur * 60);
                    std::get<2>(gridPos) = c + cur;
                    if (next >= 1) {
                        x = x + 60;
                        next--;
                        std::get<1>(gridPos) = 1;
                        std::get<2>(gridPos) = 0;
                        move(next);
                    }
                    break;
                case 1:  // Home stretch
                    if (canGoHome && id == std::get<0>(gridPos)) {
                        if (roll + c <= 6) {
                            y = y + (roll * 60);
                            std::get<2>(gridPos) = c + roll;
                            if (std::get<2>(gridPos) == 6) {
                                sem_destroy(&semToken);
                                finished = true;
                                isOut = false;
                                x = -100;
                                y = -100;
                            }
                        }
                    }
                    else {
                        next = 1;
                        x = x + (next * 60);
                        cur--;
                        std::get<1>(gridPos) = 2;
                        std::get<2>(gridPos) = 0;
                        move(cur);
                        return;
                    }
                    break;
                case 2:  // Right row
                    y = y + (cur * 60);
                    std::get<2>(gridPos) = c + cur;
                    if (next != 0) {
                        y += 60;
                        x = x + (next * 60);
                        std::get<0>(gridPos) = 2;
                        std::get<1>(gridPos) = 0;
                        std::get<2>(gridPos) = next - 1;
                    }
                    break;
                default:
                    break;
            }
            break;
        case 2:  // Right quadrant
            switch (r) {
                case 0:  // Bottom row
                    x = x + (cur * 60);
                    std::get<2>(gridPos) = c + cur;
                    if (next >= 1) {
                        y = y + 60;
                        next--;
                        std::get<1>(gridPos) = 1;
                        std::get<2>(gridPos) = 0;
                        move(next);
                    }
                    break;
                case 1:  // Home stretch
                    if (canGoHome && id == std::get<0>(gridPos)) {
                        if (roll + c <= 6) {
                            x = x - (roll * 60);
                            std::get<2>(gridPos) = c + roll;
                            if (std::get<2>(gridPos) == 6) {
                                sem_destroy(&semToken);
                                finished = true;
                                isOut = false;
                                x = -100;
                                y = -100;
                            }
                        }
                    }
                    else {
                        next = 1;
                        y = y + (next * 60);
                        cur--;
                        std::get<1>(gridPos) = 2;
                        std::get<2>(gridPos) = 0;
                        move(cur);
                        return;
                    }
                    break;
                case 2:  // Top row
                    x = x - (cur * 60);
                    std::get<2>(gridPos) = c + cur;
                    if (next != 0) {
                        x -= 60;
                        y = y + (next * 60);
                        std::get<0>(gridPos) = 3;
                        std::get<1>(gridPos) = 0;
                        std::get<2>(gridPos) = next - 1;
                    }
                    break;
                default:
                    break;
            }
            break;
        case 3:  // Bottom quadrant
            switch (r) {
                case 0:  // Right row
                    y = y + (cur * 60);
                    std::get<2>(gridPos) = c + cur;
                    if (next >= 1) {
                        x = x - 60;
                        next--;
                        std::get<1>(gridPos) = 1;
                        std::get<2>(gridPos) = 0;
                        move(next);
                    }
                    break;
                case 1:  // Home stretch
                    if (canGoHome && id == std::get<0>(gridPos)) {
                        if (roll + c <= 6) {
                            y = y - (roll * 60);
                            std::get<2>(gridPos) = c + roll;
                            if (std::get<2>(gridPos) == 6) {
                                sem_destroy(&semToken);
                                finished = true;
                                isOut = false;
                                x = -100;
                                y = -100;
                            }
                        }
                    }
                    else {
                        next = 1;
                        x = x - (next * 60);
                        cur--;
                        std::get<1>(gridPos) = 2;
                        std::get<2>(gridPos) = 0;
                        move(cur);
                        return;
                    }
                    break;
                case 2:  // Left row
                    y = y - (cur * 60);
                    std::get<2>(gridPos) = c + cur;
                    if (next != 0) {
                        y -= 60;
                        x = x - (next * 60);
                        std::get<0>(gridPos) = 0;
                        std::get<1>(gridPos) = 0;
                        std::get<2>(gridPos) = next - 1;
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
} 