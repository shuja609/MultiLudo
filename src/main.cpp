#include "../include/Game.h"
#include <cstdlib>
#include <ctime>

// Global mutex declarations
pthread_mutex_t mutex;
pthread_mutex_t mutexDice;
pthread_mutex_t mutexTurn;

void* Master(void* args) {
    Game* game = (Game*)args;
    game->Initialize();
    game->Run();
    return NULL;
}

int main(void) {
    srand(time(NULL));
    
    // Initialize mutexes
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutexDice, NULL);
    pthread_mutex_init(&mutexTurn, NULL);
    
    Game game;
    pthread_t masterThread;
    pthread_create(&masterThread, NULL, &Master, &game);
    pthread_join(masterThread, NULL);
    
    // Cleanup mutexes
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutexDice);
    pthread_mutex_destroy(&mutexTurn);
    
    return 0;
} 