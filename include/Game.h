#pragma once

#include "Player.h"
#include "raylib.h"
#include <vector>
#include <pthread.h>

class Game {
public:
    static const int SCREEN_WIDTH = 1200;
    static const int SCREEN_HEIGHT = 900;
    int screen;
    Player P1, P2, P3, P4;
    pthread_t th[4];
    bool Initial;
    std::vector<bool> FinishedThreads;
    bool WinnerScreen;
    Texture2D LudoBoard;
    Texture2D Dice[6];
    Font gameFont;

    Game();
    ~Game();
    void Initialize();
    void LoadTextures();
    void LoadGameFont();
    void InitializePlayers();
    void DrawStartScreen();
    void DrawScore(int p1, int p2, int p3, int p4);
    void DrawDice();
    void DrawWinScreen();
    void Update();
    void Run();
    void DrawTextEx(const char* text, int x, int y, int fontSize, Color color);
}; 