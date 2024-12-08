#pragma once

// - **Shuja Uddin** (22i-2553) | SE-D | FAST NUCES Islamabad
// - **Amna Hassan** (22i-8759) | SE-D | FAST NUCES Islamabad  
// - **Samra Saleem** (22i-2727) | SE-D | FAST NUCES Islamabad

#include "Player.h"
#include "raylib.h"
#include <vector>
#include <pthread.h>

/**
 * @brief Main game class that manages the Ludo board game
 * 
 * The Game class handles all core game functionality including initialization,
 * rendering, game state management, and player interactions. It coordinates 
 * between different game components and manages the game loop.
 */
class Game {
public:
    /** @brief Width of the game window in pixels */
    static const int SCREEN_WIDTH = 1200;
    
    /** @brief Height of the game window in pixels */
    static const int SCREEN_HEIGHT = 900;
    
    /** @brief Current screen/state identifier */
    int screen;
    
    /** @brief Player objects for all 4 players */
    Player P1, P2, P3, P4;
    
    /** @brief Thread handles for player operations */
    pthread_t th[4];
    
    /** @brief Flag for initial game state */
    bool Initial;
    
    /** @brief Tracks completion status of player threads */
    std::vector<bool> FinishedThreads;
    
    /** @brief Flag indicating if winner screen should be shown */
    bool WinnerScreen;
    
    /** @brief Texture for the main game board */
    Texture2D LudoBoard;
    
    /** @brief Array of dice face textures */
    Texture2D Dice[6];
    
    /** @brief Font used for game text rendering */
    Font gameFont;

    /** @brief Default constructor */
    Game();
    
    /** @brief Destructor */
    ~Game();
    
    /** @brief Initializes game state and resources */
    void Initialize();
    
    /** @brief Loads all required game textures */
    void LoadTextures();
    
    /** @brief Loads and initializes game font */
    void LoadGameFont();
    
    /** @brief Sets up player objects and their initial states */
    void InitializePlayers();
    
    /** @brief Renders the game start screen */
    void DrawStartScreen();
    
    /**
     * @brief Renders player scores
     * @param p1 Score for player 1
     * @param p2 Score for player 2
     * @param p3 Score for player 3
     * @param p4 Score for player 4
     */
    void DrawScore(int p1, int p2, int p3, int p4);
    
    /** @brief Renders the current dice state */
    void DrawDice();
    
    /** @brief Renders the winner screen */
    void DrawWinScreen();
    
    /** @brief Updates game state each frame */
    void Update();
    
    /** @brief Main game loop */
    void Run();
    
    /**
     * @brief Renders text using the game font
     * @param text Text string to render
     * @param x X-coordinate for text position
     * @param y Y-coordinate for text position
     * @param fontSize Size of the font to use
     * @param color Color of the text
     */
    void DrawTextEx(const char* text, int x, int y, int fontSize, Color color);
}; 