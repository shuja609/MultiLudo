#pragma once

#include "Token.h"
#include "raylib.h"
#include <pthread.h>

/**
 * @brief Class representing a player in the game
 * 
 * The Player class manages a player's state, tokens, and gameplay actions
 * in a board game implementation.
 */
class Player {
public:
    /** @brief Unique identifier for the player */
    int id;
    
    /** @brief Array of tokens owned by this player */
    Token* tokens;
    
    /** @brief Color associated with this player's pieces */
    Color color;
    
    /** @brief Flag indicating if player has completed the game */
    bool completed;
    
    /** @brief Player's current score in the game */
    int score;
    
    /** @brief Flag indicating if it's this player's turn */
    bool isPlaying;

    /** @brief Default constructor */
    Player();
    
    /** @brief Destructor */
    ~Player();
    
    /**
     * @brief Parameterized constructor
     * @param i Player ID
     * @param c Player color
     * @param t Texture for player tokens
     */
    Player(int i, Color c, Texture2D t);
    
    /**
     * @brief Initialize player with given parameters
     * @param i Player ID
     * @param c Player color
     * @param t Texture for player tokens
     */
    void setPlayer(int i, Color c, Texture2D t);
    
    /** @brief Check and update player's current play state */
    void checkPlayState();
    
    /** @brief Initialize player for game start */
    void Start();
    
    /** @brief Allow player tokens to return to home position */
    void allowHome();
    
    /**
     * @brief Handle token collision events
     * @param movedToken Index of the token that moved
     */
    void collision(int movedToken);
    
    /** @brief Execute dice roll for player's turn */
    void rollDice();
    
    /** @brief Handle token movement based on dice roll */
    void move();
}; 