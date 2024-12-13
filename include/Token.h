#pragma once

#include "raylib.h"
#include <tuple>
#include <semaphore.h>

/**
 * @brief Class representing an individual token in the Ludo game
 * 
 * The Token class manages the state and behavior of a single game token,
 * including its position, movement, and status on the board.
 */
class Token {
public:
    int id;                             ///< Unique identifier for the token
    int gridID;                         ///< Current position ID on the game grid
    std::tuple<int, int, int> gridPos;  ///< 3D coordinates for token position (x,y,z)
    int x, y;                           ///< Current pixel coordinates on screen
    int initX, initY;                   ///< Initial/home position coordinates
    bool isSafe;                        ///< Flag indicating if token is on a safe spot
    bool canGoHome;                     ///< Flag indicating if token can enter home stretch
    bool finished;                      ///< Flag indicating if token has reached home
    bool isOut;                         ///< Flag indicating if token is out of starting area
    sem_t semToken;                     ///< Semaphore for thread-safe token operations
    Texture2D token;                    ///< Token's visual texture

    /**
     * @brief Default constructor
     * Initializes a token with default values
     */
    Token();

    /**
     * @brief Sets the token's texture
     * @param t Texture2D object to be used for the token
     */
    void setTexture(Texture2D t);

    /**
     * @brief Sets the token's starting position
     * @param i Initial position identifier
     */
    void setStart(int i);

    /**
     * @brief Updates the token's grid position
     * Recalculates coordinates based on current grid ID
     */
    void updateGrid();

    /**
     * @brief Draws the token in its initial position
     */
    void drawInit();

    /**
     * @brief Moves token out of starting area
     * Called when a player rolls a 6
     */
    void outToken();

    /**
     * @brief Returns token to starting area
     * Called when token is captured
     */
    void inToken();

    /**
     * @brief Moves the token on the board
     * @param roll Number of spaces to move
     */
    void move(int roll);
}; 