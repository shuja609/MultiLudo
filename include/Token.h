#pragma once

#include "raylib.h"
#include <tuple>
#include <semaphore.h>

class Token {
public:
    int id;
    int gridID;
    std::tuple<int, int, int> gridPos;
    int x, y;
    int initX, initY;
    bool isSafe;
    bool canGoHome;
    bool finished;
    bool isOut;
    sem_t semToken;
    Texture2D token;

    Token();
    void setTexture(Texture2D t);
    void setStart(int i);
    void updateGrid();
    void drawInit();
    void outToken();
    void inToken();
    void move(int roll);
}; 