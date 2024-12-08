#include "../include/Token.h"
#include "../include/Utils.h"
#include <iostream>

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

void Token::setStart(int i) {
    id = i;
    switch (id) {
        case 0:
            x = 60;
            y = 360;
            break;
        case 1:
            x = 480;
            y = 60;
            break;
        case 2:
            x = 780;
            y = 480;
            break;
        case 3:
            x = 360;
            y = 780;
            break;
        default:
            break;
    }
}

void Token::updateGrid() {
    LudoGrid[id][gridID] = gridPos;
    if (isTokenSafe(gridPos))
        isSafe = true;
    else
        isSafe = false;
}

void Token::drawInit() {
    if (isOut == false && !finished)
        DrawTexture(token, initX, initY, WHITE);
    else {
        DrawTexture(token, x, y, WHITE);
    }
}

void Token::outToken() {
    sem_post(&semToken);
    isOut = true;
    gridPos = std::make_tuple(id, 2, 1);
    DrawTexture(token, x, y, WHITE);
}

void Token::inToken() {
    sem_wait(&semToken);
    isOut = false;
    setStart(id);
    gridPos = std::make_tuple(-1, -1, -1);
}

void Token::move(int roll) {
    if (roll == 0)
        return;
    std::cout << "\nDice Val " << roll << std::endl;
    int g = std::get<0>(gridPos);
    int r = std::get<1>(gridPos);
    int c = std::get<2>(gridPos);
    std::cout << "Grid: " << g
         << " Row: " << r
         << " Col: " << c << std::endl;
    int next = 0, cur = 0;
    std::cout << "C+Roll " << c + roll << std::endl;
    if (c + roll >= 5) {
        next = (c + roll) - 5;
        cur = roll - next;
        std::cout << "Next: " << next
             << " Cur: " << cur << std::endl;
    }
    else {
        cur = roll;
    }
    switch (g) {
        case 0:
            switch (r) {
                case 0:
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
                case 1:
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
                case 2:
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
        case 1:
            switch (r) {
                case 0:
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
                case 1:
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
                case 2:
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
        case 2:
            switch (r) {
                case 0:
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
                case 1:
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
                case 2:
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
        case 3:
            switch (r) {
                case 0:
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
                case 1:
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
                case 2:
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