/**
 * @file main.cpp
 * @brief Entry point for the multiplayer board game implementation
 * 
 * This file contains the main game initialization and thread management.
 * The game uses multiple threads to handle player turns concurrently,
 * with synchronization mechanisms to ensure thread safety.
 */

#include "../include/Game.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

// Synchronization primitives for thread-safe game state management
pthread_mutex_t gameStateMutex;      // Controls access to shared game state
pthread_mutex_t diceRollMutex;       // Controls access to dice rolling mechanism
pthread_mutex_t turnControlMutex;    // Controls access to turn management

/**
 * @brief Main game controller thread function
 * 
 * Handles the initialization and execution of the main game loop.
 * This function runs in a separate thread to manage the game state
 * while allowing concurrent player actions.
 * 
 * @param gameInstance Pointer to the main game instance
 * @return void pointer as required by pthread API
 */
void* GameController(void* gameInstance) {
    Game* gamePtr = static_cast<Game*>(gameInstance);
    
    // Initialize game systems and resources
    gamePtr->Initialize();
    
    // Start the main game loop
    gamePtr->Run();
    
    return nullptr;
}

/**
 * @brief Program entry point
 * 
 * Initializes the game environment, creates necessary threads,
 * and manages the game lifecycle.
 */
int main() {
    // Initialize random number generator with current time
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Initialize synchronization primitives
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
    
    // Create main game instance
    Game gameInstance;
    
    // Create and start game controller thread
    pthread_t controllerThread;
    if (pthread_create(&controllerThread, nullptr, &GameController, &gameInstance) != 0) {
        return 1;  // Failed to create thread
    }
    
    // Wait for game controller thread to complete
    pthread_join(controllerThread, nullptr);
    
    // Cleanup synchronization primitives
    pthread_mutex_destroy(&gameStateMutex);
    pthread_mutex_destroy(&diceRollMutex);
    pthread_mutex_destroy(&turnControlMutex);
    
    return 0;
}