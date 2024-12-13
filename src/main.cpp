/**
 * @file main.cpp
 * @brief Main entry point for the Ludo game application
 *
 * This file contains the main game initialization, thread management,
 * and program entry point for a multi-threaded Ludo board game implementation.
 *
 * @authors
 * - Shuja Uddin (22i2553)
 * - Amna Hassan (22i8759) 
 * - Samra Saleem (22i----)
 */

#include "../include/Game.h"
#include <cstdlib>
#include <ctime>

/**
 * @brief Global mutex for protecting shared game state
 * Used to synchronize access to game board and player states
 */
pthread_mutex_t mutex;

/**
 * @brief Global mutex for protecting dice rolling
 * Ensures atomic dice roll operations between threads
 */
pthread_mutex_t mutexDice;

/**
 * @brief Global mutex for protecting turn management
 * Controls player turn order and prevents simultaneous turns
 */
pthread_mutex_t mutexTurn;

/**
 * @brief Master thread function that controls the main game loop
 *
 * Initializes the game state and runs the main game loop in a separate thread.
 * This allows for concurrent handling of player actions and game state updates.
 *
 * @param args Pointer to Game instance cast as void*
 * @return NULL on completion
 */
void* Master(void* args) {
    Game* game = (Game*)args;
    game->Initialize();
    game->Run();
    return NULL;
}

/**
 * @brief Main program entry point
 *
 * Initializes the random number generator, creates and manages the game threads,
 * and handles proper cleanup of system resources.
 *
 * @return 0 on successful execution
 */
int main(void) {
    // Seed random number generator for dice rolls
    srand(time(NULL));
    
    // Initialize synchronization mutexes
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutexDice, NULL);
    pthread_mutex_init(&mutexTurn, NULL);
    
    // Create and start the master game thread
    Game game;
    pthread_t masterThread;
    pthread_create(&masterThread, NULL, &Master, &game);
    pthread_join(masterThread, NULL);
    
    // Cleanup and destroy mutexes
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutexDice);
    pthread_mutex_destroy(&mutexTurn);
    
    return 0;
}