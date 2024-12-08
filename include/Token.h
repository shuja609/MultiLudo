#pragma once

#include "raylib.h"
#include <tuple>
#include <semaphore.h>

/**
 * @brief Class representing a game token/piece in the Ludo board game
 * 
 * The Token class manages individual game pieces, their positions, movement rules,
 * and state on the game board. It handles both the visual representation and logical
 * positioning of tokens.
 */
class Token {
public:
    /** @brief Unique identifier for the token */
    int id;
    
    /** @brief Grid identifier for token position tracking */
    int gridID;
    
    /** @brief Current position in the game grid as (row, column, grid number) */
    std::tuple<int, int, int> gridPos;
    
    /** @brief Current pixel coordinates on screen */
    int x, y;
    
    /** @brief Initial/home position pixel coordinates */
    int initX, initY;
    
    /** @brief Flag indicating if token is on a safe spot */
    bool isSafe;
    
    /** @brief Flag indicating if token can return home */
    bool canGoHome;
    
    /** @brief Flag indicating if token has reached final position */
    bool finished;
    
    /** @brief Flag indicating if token is out of starting area */
    bool isOut;
    
    /** @brief Semaphore for thread-safe token operations */
    sem_t semToken;
    
    /** @brief Visual texture for rendering the token */
    Texture2D token;

    /** @brief Default constructor */
    Token();
    
    /**
     * @brief Sets the token's texture for rendering
     * @param t The texture to assign to this token
     */
    void setTexture(Texture2D t);
    
    /**
     * @brief Initializes starting position based on player ID
     * @param i Player identifier
     */
    void setStart(int i);
    
    /** @brief Updates token's grid position in game state */
    void updateGrid();
    
    /** @brief Initializes token drawing parameters */
    void drawInit();
    
    /** @brief Moves token out of starting area */
    void outToken();
    
    /** @brief Returns token to starting area */
    void inToken();
    
    /**
     * @brief Moves token based on dice roll
     * @param roll Number of spaces to move
     */
    void move(int roll);

private:
    /**
     * @brief Movement validation functions
     */
    bool canMoveToPosition(const std::tuple<int, int, int>& newPos) const;
    void updatePosition(int newX, int newY);
    bool isValidMove(int roll) const;
    void handleHomeStretch(int roll);
    void handleNormalPath(int roll);
    
    /**
     * @brief Grid-specific movement handlers for each quadrant
     * @param r Row coordinate
     * @param c Column coordinate
     * @param cur Current position
     * @param next Next position
     */
    void handleGrid0Movement(int r, int c, int cur, int next);
    void handleGrid1Movement(int r, int c, int cur, int next);
    void handleGrid2Movement(int r, int c, int cur, int next);
    void handleGrid3Movement(int r, int c, int cur, int next);
    
    /**
     * @brief Position calculation and validation helpers
     */
    std::tuple<int, int, int> calculateNextPosition(int roll) const;
    bool isHomeStretchEntry(const std::tuple<int, int, int>& pos) const;
    bool isIntersection(const std::tuple<int, int, int>& pos) const;
    
    /**
     * @brief State validation methods
     */
    bool isValidGridPosition(const std::tuple<int, int, int>& pos) const;
    void validateMove(int roll) const;
}; 