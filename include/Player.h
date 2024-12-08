#pragma once

#include "Token.h"
#include "raylib.h"
#include <pthread.h>

class Player {
public:
    int id;
    Token* tokens;
    Color color;
    bool completed;
    int score;
    bool isPlaying;

    Player();
    ~Player();
    Player(int i, Color c, Texture2D t);
    void setPlayer(int i, Color c, Texture2D t);
    void checkPlayState();
    void Start();
    void allowHome();
    void collision(int movedToken);
    void rollDice();
    void move();
}; 