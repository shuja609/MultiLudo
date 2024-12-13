#pragma once

#include "Player.h"
#include "raylib.h"
#include <vector>
#include <pthread.h>

/**
 * @brief Main game class that handles core game functionality and rendering
 * 
 * The Game class manages the overall game state, window rendering, player management,
 * and game flow control. It coordinates between different game screens and handles
 * asset loading and resource management.
 */
class Game {
public:
    static const int SCREEN_WIDTH = 1200;   ///< Window width in pixels
    static const int SCREEN_HEIGHT = 900;   ///< Window height in pixels
    int screen;                            ///< Current game screen/state identifier
    Player P1, P2, P3, P4;                 ///< Player objects for all 4 players
    pthread_t th[4];                       ///< Thread handles for player threads
    bool Initial;                          ///< Flag indicating initial game state
    std::vector<bool> FinishedThreads;     ///< Tracks completion status of player threads
    bool WinnerScreen;                     ///< Flag for displaying winner screen
    Texture2D LudoBoard;                   ///< Main game board texture
    Texture2D Dice[6];                     ///< Array of dice face textures
    Font gameFont;                         ///< Font used for game text

    /**
     * @brief Default constructor
     * Initializes game instance and basic parameters
     */
    Game();

    /**
     * @brief Destructor
     * Cleans up resources and frees memory
     */
    ~Game();

    /**
     * @brief Initializes core game components and states
     */
    void Initialize();

    /**
     * @brief Loads all required game textures into memory
     */
    void LoadTextures();

    /**
     * @brief Loads and initializes the game font
     */
    void LoadGameFont();

    /**
     * @brief Sets up initial state for all players
     */
    void InitializePlayers();

    /**
     * @brief Renders the game's start/menu screen
     */
    void DrawStartScreen();

    /**
     * @brief Displays current score for all players
     * @param p1 Player 1's score
     * @param p2 Player 2's score
     * @param p3 Player 3's score
     * @param p4 Player 4's score
     */
    void DrawScore(int p1, int p2, int p3, int p4);

    /**
     * @brief Renders the current dice state
     */
    void DrawDice();

    /**
     * @brief Displays the winner screen with final results
     */
    void DrawWinScreen();

    /**
     * @brief Updates game state each frame
     */
    void Update();

    /**
     * @brief Main game loop that controls game flow
     */
    void Run();

    /**
     * @brief Utility function for drawing text with specific parameters
     * @param text Text string to display
     * @param x X-coordinate position
     * @param y Y-coordinate position
     * @param fontSize Size of the font
     * @param color Text color
     */
    void DrawTextEx(const char* text, int x, int y, int fontSize, Color color);
}; 