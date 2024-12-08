/**
 * @file main.cpp
 * @brief Entry point for the multiplayer Ludo board game implementation
 * @details This file implements the core game initialization, thread management,
 * and synchronization mechanisms. The game architecture uses multiple threads
 * to handle concurrent player turns while maintaining thread safety through
 * mutex-based synchronization.
 * @author Your Name
 * @date YYYY-MM-DD
 */

#include "../include/Game.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

/**
 * @brief Mutex for protecting shared game state access
 * @details Controls concurrent access to game variables and state that
 * could be modified by multiple threads simultaneously
 */
pthread_mutex_t gameStateMutex;

/**
 * @brief Mutex for dice rolling mechanism
 * @details Ensures atomic dice roll operations when multiple players
 * attempt to roll dice concurrently
 */
pthread_mutex_t diceRollMutex;

/**
 * @brief Mutex for turn management
 * @details Controls player turn transitions and prevents race conditions
 * in turn-based gameplay mechanics
 */
pthread_mutex_t turnControlMutex;

/**
 * @brief Main game controller thread function
 * @param gameInstance Pointer to the main Game object instance
 * @return void* Required by pthread API, always returns nullptr
 * @throws None
 * @details This function executes in a dedicated thread and manages the primary
 * game loop. It handles:
 * - Game system initialization
 * - Resource loading
 * - Main game loop execution
 * - Game state management
 */
void* GameController(void* gameInstance) {
    Game* gamePtr = static_cast<Game*>(gameInstance);
    
    // Initialize core game systems and load resources
    gamePtr->Initialize();
    
    // Enter main game loop
    gamePtr->Run();
    
    return nullptr;
}

/**
 * @brief Program entry point
 * @return int Exit status (0 for success, 1 for initialization failure)
 * @details Main function responsible for:
 * - Initializing random number generator
 * - Setting up synchronization primitives
 * - Creating and managing the game controller thread
 * - Proper cleanup of resources
 * 
 * The function implements proper error handling for thread and mutex initialization,
 * ensuring clean shutdown in case of failures.
 */
int main() {
    // Seed random number generator with current timestamp
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Initialize mutex objects with error handling
    if (pthread_mutex_init(&gameStateMutex, nullptr) != 0) {
        std::cerr << "Error: Failed to initialize game state mutex" << std::endl;
        return 1;
    }
    if (pthread_mutex_init(&diceRollMutex, nullptr) != 0) {
        std::cerr << "Error: Failed to initialize dice roll mutex" << std::endl;
        pthread_mutex_destroy(&gameStateMutex); // Clean up previously initialized mutex
        return 1;
    }
    if (pthread_mutex_init(&turnControlMutex, nullptr) != 0) {
        std::cerr << "Error: Failed to initialize turn control mutex" << std::endl;
        pthread_mutex_destroy(&gameStateMutex); // Clean up previously initialized mutexes
        pthread_mutex_destroy(&diceRollMutex);
        return 1;
    }
    
    // Instantiate main game object
    Game gameInstance;
    
    // Create and launch game controller thread
    pthread_t controllerThread;
    if (pthread_create(&controllerThread, nullptr, &GameController, &gameInstance) != 0) {
        std::cerr << "Error: Failed to create game controller thread" << std::endl;
        return 1;
    }
    
    // Block until game controller thread completes execution
    pthread_join(controllerThread, nullptr);
    
    // Clean up synchronization primitives
    pthread_mutex_destroy(&gameStateMutex);
    pthread_mutex_destroy(&diceRollMutex);
    pthread_mutex_destroy(&turnControlMutex);
    
    return 0;
}