
// - **Shuja Uddin** (22i-2553) | SE-D | FAST NUCES Islamabad
// - **Amna Hassan** (22i-8759) | SE-D | FAST NUCES Islamabad  
// - **Samra Saleem** (22i-2727) | SE-D | FAST NUCES Islamabad



/**
 * @file Player.cpp
 * @brief Implementation of the Player class for the Ludo game
 */

#include "../include/Player.h"
#include "../include/Utils.h"
#include <algorithm>
#include <iostream>

// External mutex declarations for thread synchronization
extern pthread_mutex_t diceRollMutex;
extern pthread_mutex_t turnControlMutex;

/**
 * @brief Default constructor
 * Initializes a Player object with default values
 */
Player::Player() : tokens(nullptr), score(0), completed(false), isPlaying(false) {}

/**
 * @brief Destructor
 * Cleans up dynamically allocated token array
 */
Player::~Player() {
    if (tokens != nullptr) {
        delete[] tokens;
        tokens = nullptr;
    }
}

/**
 * @brief Parameterized constructor
 * @param i Player ID
 * @param c Player color
 * @param t Texture for player tokens
 */
Player::Player(int i, Color c, Texture2D t) {
    score = 0;
    completed = false;
    id = i;
    color = c;
    isPlaying = false;
    
    if (numTokens > 0) {
        // Allocate and initialize tokens
        tokens = new Token[numTokens];
        for (int k = 0; k < numTokens; k++) {
            tokens[k].setTexture(t);
            tokens[k].setStart(id);
            tokens[k].gridID = k;
        }

        // Initialize token positions based on player ID
        int arr[4][4][2] = {
            {{90, 90}, {200, 90}, {90, 200}, {200, 200}},     // Player 0 positions
            {{630, 90}, {740, 90}, {630, 200}, {740, 200}},   // Player 1 positions
            {{630, 630}, {740, 740}, {630, 740}, {740, 630}}, // Player 2 positions
            {{90, 630}, {200, 630}, {90, 740}, {200, 740}}    // Player 3 positions
        };

        for (int k = 0; k < numTokens; k++) {
            tokens[k].initX = arr[id][k][0];
            tokens[k].initY = arr[id][k][1];
        }
    } else {
        tokens = nullptr;
    }
}

/**
 * @brief Reinitializes a player with new parameters
 * @param i Player ID
 * @param c Player color
 * @param t Texture for player tokens
 */
void Player::setPlayer(int i, Color c, Texture2D t) {
    completed = false;
    score = 0;
    id = i;
    color = c;
    isPlaying = false;

    if (numTokens > 0) {
        delete[] tokens;  // Clean up any existing tokens
        tokens = new Token[numTokens];

        // Initialize new tokens
        for (int k = 0; k < numTokens; k++) {
            tokens[k].setTexture(t);
            tokens[k].setStart(id);
            tokens[k].gridID = k;
        }

        // Set initial positions for tokens
        int arr[4][4][2] = {
            {{90, 90}, {200, 90}, {90, 200}, {200, 200}},     // Player 0 positions
            {{630, 90}, {740, 90}, {630, 200}, {740, 200}},   // Player 1 positions
            {{630, 630}, {740, 740}, {630, 740}, {740, 630}}, // Player 2 positions
            {{90, 630}, {200, 630}, {90, 740}, {200, 740}}    // Player 3 positions
        };

        for (int k = 0; k < numTokens; k++) {
            tokens[k].initX = arr[id][k][0];
            tokens[k].initY = arr[id][k][1];
        }
    }
}

/**
 * @brief Sets all tokens to "out" state for testing
 */
void Player::checkPlayState() {
    for (int i = 0; i < numTokens; i++) {
        tokens[i].isOut = true;
    }
}

/**
 * @brief Initializes and updates the game state for all tokens
 * Handles token initialization, grid updates, and win conditions
 */
void Player::Start() {
    bool found = false;
    bool finCheck = false;
    for (int i = 0; i < numTokens; i++) {
        // Reset tokens that were knocked out
        if (LudoGrid[id][i] == std::make_tuple(-2, -2, -2)) {
          //  std::cout << "Found this as well" << std::endl;
            tokens[i].inToken();
            LudoGrid[id][i] = std::make_tuple(-1, -1, -1);
        }
        tokens[i].drawInit();
        tokens[i].updateGrid();
        
        // Check completion status
        if (tokens[i].finished == false) {
            finCheck = true;
        }
        if (tokens[i].isOut == true) {
            found = true;
        }
    }
    
    // Handle player completion
    if (!finCheck) {
        completed = true;
        //std::cout << "THE PLAYER HAS COMPLETED" << std::endl;
        score++;
        winners.push_back(id + 1);
    }
    if (!found) {
        isPlaying = false;
    }
}

/**
 * @brief Enables home movement for all tokens
 */
void Player::allowHome() {
    for (int i = 0; i < numTokens; i++) {
        tokens[i].canGoHome = true;
    }
}

/**
 * @brief Handles collision detection and resolution between tokens
 * @param movedToken Index of the token that was just moved
 */
void Player::collision(int movedToken) {
    // Skip collision check if token is on a safe spot
    if (tokens[movedToken].isSafe) {
     //   std::cout << "Moved Token: " << movedToken << " is on a Safe Spot" << std::endl;
        return;
    }
    
    // Check for collisions with other players' tokens
    for (int pid = 0; pid < 4; pid++) {
        for (int tokenId = 0; tokenId < numTokens; tokenId++) {
            if (LudoGrid[pid][tokenId] == tokens[movedToken].gridPos && pid != id) {
                allowHome();
                score++;
                LudoGrid[pid][tokenId] = std::make_tuple(-2, -2, -2);
                // Debug information
                // std::cout << "found collison" << std::endl;
                // std::cout << "pid: " << pid << std::endl;
                // std::cout << "tokenID: " << tokenId << std::endl;
                // std::cout << "movedToken: " << movedToken << std::endl;
                // std::cout << "id: " << id << std::endl;
            }
        }
    }
}

/**
 * @brief Handles dice rolling mechanics and turn management
 * Implements the dice rolling logic with mutex protection for thread safety
 */
void Player::rollDice() {
    if (moveDice == true) {
        pthread_mutex_lock(&diceRollMutex);
        if (id == turn - 1 && movePlayer == false && !completed) {
            if (completed) {
                turn = getTurn();
                pthread_mutex_unlock(&diceRollMutex);
                return;
            }
            Rectangle diceRec = {990, 500, 108.0, 108.0};
            while(!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (CheckCollisionPointRec(GetMousePosition(), diceRec)) {
                        dice = (rand() % 6) + 1;
                        diceCount++;
                        
                        // Handle three consecutive sixes
                        if (diceCount == 3 && dice == 6) {
                            pthread_mutex_lock(&turnControlMutex);
                            diceVal.resize(3);
                            std::fill(diceVal.begin(), diceVal.end(), 0);
                            turn = getTurn();
                            diceCount = 0;
                            lastTurn = turn;
                            pthread_mutex_unlock(&turnControlMutex);
                            pthread_mutex_unlock(&diceRollMutex);
                            return;
                        }
                        
                        // Handle rolling a six
                        if (dice == 6) {
                            diceVal[diceCount - 1] = dice;
                            lastTurn = turn;
                            pthread_mutex_unlock(&diceRollMutex);
                            return;
                        }
                        else {
                            diceVal[diceCount - 1] = dice;
                            if (isPlaying == true || diceVal[0] == 6) {
                                movePlayer = true;
                                moveDice = false;
                                lastTurn = turn;
                                pthread_mutex_unlock(&diceRollMutex);
                                return;
                            }
                            else {
                                pthread_mutex_lock(&turnControlMutex);
                                diceVal.resize(3);
                                std::fill(diceVal.begin(), diceVal.end(), 0);
                                turn = getTurn();
                                lastTurn = turn;
                                pthread_mutex_unlock(&turnControlMutex);
                            }
                            diceCount = 0;
                        }
                    }
                }
            }
        }
        pthread_mutex_unlock(&diceRollMutex);
    }
}

/**
 * @brief Handles token movement based on dice rolls and user input
 * Manages token movement, collisions, and turn transitions
 */
void Player::move() {
    bool found = false;
    if (movePlayer == true && lastTurn - 1 == id) {
        for (int i = 0; i < numTokens; i++) {
            // Create clickable area for token
            Rectangle diceRec;
            if (tokens[i].isOut) {
                diceRec = {(float)tokens[i].x, (float)tokens[i].y, 60.0, 60.0};
            }
            else {
                diceRec = {(float)tokens[i].initX, (float)tokens[i].initY, 60.0, 60.0};
            }
            
            // Handle token selection and movement
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(GetMousePosition(), diceRec)) {
                    found = true;
                    // Handle token leaving home
                    if (tokens[i].isOut == false && diceVal[0] == 6) {
                        tokens[i].outToken();
                        isPlaying = true;
                        tokens[i].updateGrid();
                        diceVal.erase(diceVal.begin());
                    }
                    // Handle normal token movement
                    else if (tokens[i].isOut == true) {
                        tokens[i].move(diceVal[0]);
                        tokens[i].updateGrid();
                        collision(i);
                        diceVal.erase(diceVal.begin());
                        
                        // Handle turn completion
                        if (diceVal.empty() || diceVal[0] == 0) {
                            pthread_mutex_lock(&turnControlMutex);
                            turn = getTurn();
                            lastTurn = turn;
                            diceVal.resize(3);
                            std::fill(diceVal.begin(), diceVal.end(), 0);
                            movePlayer = false;
                            moveDice = true;
                            diceCount = 0;
                            pthread_mutex_unlock(&turnControlMutex);
                        }
                    }
                }
            }
            if (found)
                break;
        }
    }
} 