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

private:
    // Helper functions for movement
    bool canMoveToPosition(const std::tuple<int, int, int>& newPos) const;
    void updatePosition(int newX, int newY);
    bool isValidMove(int roll) const;
    void handleHomeStretch(int roll);
    void handleNormalPath(int roll);
    
    // Grid movement handlers
    void handleGrid0Movement(int r, int c, int cur, int next);
    void handleGrid1Movement(int r, int c, int cur, int next);
    void handleGrid2Movement(int r, int c, int cur, int next);
    void handleGrid3Movement(int r, int c, int cur, int next);
    
    // Grid position helpers
    std::tuple<int, int, int> calculateNextPosition(int roll) const;
    bool isHomeStretchEntry(const std::tuple<int, int, int>& pos) const;
    bool isIntersection(const std::tuple<int, int, int>& pos) const;
    
    // State validation
    bool isValidGridPosition(const std::tuple<int, int, int>& pos) const;
    void validateMove(int roll) const;
}; 