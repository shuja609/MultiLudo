#include "../include/Player.h"
#include "../include/Utils.h"
#include <algorithm>
#include <iostream>

extern pthread_mutex_t diceRollMutex;
extern pthread_mutex_t turnControlMutex;

Player::Player() : tokens(nullptr), score(0), completed(false), isPlaying(false) {}

Player::~Player() {
    if (tokens != nullptr) {
        delete[] tokens;
        tokens = nullptr;
    }
}

Player::Player(int i, Color c, Texture2D t) {
    score = 0;
    completed = false;
    id = i;
    color = c;
    isPlaying = false;
    
    if (numTokens > 0) {
        tokens = new Token[numTokens];
        for (int k = 0; k < numTokens; k++) {
            tokens[k].setTexture(t);
            tokens[k].setStart(id);
            tokens[k].gridID = k;
        }

        int arr[4][4][2] = {
            {{90, 90}, {200, 90}, {90, 200}, {200, 200}},
            {{630, 90}, {740, 90}, {630, 200}, {740, 200}},
            {{630, 630}, {740, 740}, {630, 740}, {740, 630}},
            {{90, 630}, {200, 630}, {90, 740}, {200, 740}}
        };

        for (int k = 0; k < numTokens; k++) {
            tokens[k].initX = arr[id][k][0];
            tokens[k].initY = arr[id][k][1];
        }
    } else {
        tokens = nullptr;
    }
}

void Player::setPlayer(int i, Color c, Texture2D t) {
    completed = false;
    score = 0;
    id = i;
    color = c;
    isPlaying = false;

    if (numTokens > 0) {
        delete[] tokens;  // Clean up any existing tokens
        tokens = new Token[numTokens];

        for (int k = 0; k < numTokens; k++) {
            tokens[k].setTexture(t);
            tokens[k].setStart(id);
            tokens[k].gridID = k;
        }

        int arr[4][4][2] = {
            {{90, 90}, {200, 90}, {90, 200}, {200, 200}},
            {{630, 90}, {740, 90}, {630, 200}, {740, 200}},
            {{630, 630}, {740, 740}, {630, 740}, {740, 630}},
            {{90, 630}, {200, 630}, {90, 740}, {200, 740}}
        };

        for (int k = 0; k < numTokens; k++) {
            tokens[k].initX = arr[id][k][0];
            tokens[k].initY = arr[id][k][1];
        }
    }
}

void Player::checkPlayState() {
    for (int i = 0; i < numTokens; i++) {
        tokens[i].isOut = true;
    }
}

void Player::Start() {
    bool found = false;
    bool finCheck = false;
    for (int i = 0; i < numTokens; i++) {
        if (LudoGrid[id][i] == std::make_tuple(-2, -2, -2)) {
            std::cout << "Found this as well" << std::endl;
            tokens[i].inToken();
            LudoGrid[id][i] = std::make_tuple(-1, -1, -1);
        }
        tokens[i].drawInit();
        tokens[i].updateGrid();
        if (tokens[i].finished == false) {
            finCheck = true;
        }
        if (tokens[i].isOut == true) {
            found = true;
        }
    }
    if (!finCheck) {
        completed = true;
        std::cout << "THE PLAYER HAS COMPLETED" << std::endl;
        score++;
        winners.push_back(id + 1);
    }
    if (!found) {
        isPlaying = false;
    }
}

void Player::allowHome() {
    for (int i = 0; i < numTokens; i++) {
        tokens[i].canGoHome = true;
    }
}

void Player::collision(int movedToken) {
    if (tokens[movedToken].isSafe) {
        std::cout << "Moved Token: " << movedToken << " is on a Safe Spot" << std::endl;
        return;
    }
    for (int pid = 0; pid < 4; pid++) {
        for (int tokenId = 0; tokenId < numTokens; tokenId++) {
            if (LudoGrid[pid][tokenId] == tokens[movedToken].gridPos && pid != id) {
                allowHome();
                score++;
                LudoGrid[pid][tokenId] = std::make_tuple(-2, -2, -2);
                std::cout << "found collison" << std::endl;
                std::cout << "pid: " << pid << std::endl;
                std::cout << "tokenID: " << tokenId << std::endl;
                std::cout << "movedToken: " << movedToken << std::endl;
                std::cout << "id: " << id << std::endl;
            }
        }
    }
}

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

void Player::move() {
    bool found = false;
    if (movePlayer == true && lastTurn - 1 == id) {
        for (int i = 0; i < numTokens; i++) {
            Rectangle diceRec;
            if (tokens[i].isOut) {
                diceRec = {(float)tokens[i].x, (float)tokens[i].y, 60.0, 60.0};
            }
            else {
                diceRec = {(float)tokens[i].initX, (float)tokens[i].initY, 60.0, 60.0};
            }
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(GetMousePosition(), diceRec)) {
                    found = true;
                    if (tokens[i].isOut == false && diceVal[0] == 6) {
                        tokens[i].outToken();
                        isPlaying = true;
                        tokens[i].updateGrid();
                        diceVal.erase(diceVal.begin());
                    }
                    else if (tokens[i].isOut == true) {
                        tokens[i].move(diceVal[0]);
                        tokens[i].updateGrid();
                        collision(i);
                        diceVal.erase(diceVal.begin());
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