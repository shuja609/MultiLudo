#pragma once

#include "Token.h"
#include "raylib.h"
#include <pthread.h>

/**
 * @brief Class representing a player in the Ludo game
 * 
 * The Player class manages an individual player's state, tokens, and game actions.
 * It handles player initialization, token movement, dice rolling, and win conditions.
 */
class Player {
public:
    int id;                 ///< Unique identifier for the player (1-4)
    Token* tokens;          ///< Array of tokens owned by this player
    Color color;           ///< Player's color for visual representation
    bool completed;        ///< Flag indicating if player has won
    int score;            ///< Player's current score
    bool isPlaying;       ///< Flag indicating if player is active in game

    /**
     * @brief Default constructor
     * Initializes a player with default values
     */
    Player();

    /**
     * @brief Destructor
     * Cleans up player resources and token array
     */
    ~Player();

    /**
     * @brief Parameterized constructor
     * @param i Player ID
     * @param c Player color
     * @param t Token texture
     */
    Player(int i, Color c, Texture2D t);

    /**
     * @brief Configures player parameters
     * @param i Player ID to set
     * @param c Player color to set
     * @param t Token texture to use
     */
    void setPlayer(int i, Color c, Texture2D t);

    /**
     * @brief Verifies if player can continue playing
     * Updates isPlaying flag based on token positions
     */
    void checkPlayState();

    /**
     * @brief Initiates player's turn
     * Handles initial turn setup and validation
     */
    void Start();

    /**
     * @brief Enables tokens to move to home position
     * Validates and permits home square movement
     */
    void allowHome();

    /**
     * @brief Handles token collision with other players
     * @param movedToken Index of token that caused collision
     */
    void collision(int movedToken);

    /**
     * @brief Handles dice rolling mechanism
     * Generates and validates dice roll result
     */
    void rollDice();

    /**
     * @brief Processes token movement
     * Handles token selection and movement validation
     */
    void move();
}; 