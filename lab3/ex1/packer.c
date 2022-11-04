#include <semaphore.h>
#include <stdio.h>

#include "packer.h"

// You can declare global variables here
typedef struct {
    int *first;
    int *second;
    sem_t mutex;
    sem_t waiting;
    sem_t first_ball_queue;
} barrier;

barrier bar[3];

void packer_init(void) {
    // Write initialization code here (called once at the start of the program).
    for (int i = 0; i < 3; i++) {
        bar[i].first = NULL;
        bar[i].second = NULL;
        sem_init(&bar[i].mutex, 0, 1);
        sem_init(&bar[i].waiting, 0, 0);
        sem_init(&bar[i].first_ball_queue, 0, 1);
    }
}

void packer_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).
    for (int i = 0; i < 3; i++) {
        sem_destroy(&bar[i].mutex);
        sem_destroy(&bar[i].waiting);
        sem_destroy(&bar[i].first_ball_queue);
    }
}

int pack_ball(int colour, int id) {
    // Write your code here.
    barrier *b = &bar[colour - 1];
    // variable to deal with 1b
    int own = id;
    int other = id;
    // printf("Ball reached: %d\n", id);
    sem_wait(&b->mutex);
    if (b->first == NULL) { // First ball to arrive
        // 1. Stores the pointer to own id in b->first, stores the pointer to 'other' in b->second
        b->first = &own;
        b->second = &other;
        
        // Semaphore to ensure the order of the waiting balls, since the thread to unblock next after sema_post(&b->waiting) may not be based on FIFO
        sem_wait(&b->first_ball_queue);
        sem_post(&b->mutex);
        // 2. Waits for other ball to arrive
        // printf("Ball waiting: %d\n", id);
        sem_wait(&b->waiting); 
        // printf("first ball thru: %d\n", id);
        sem_post(&b->first_ball_queue);
    } else { // Second ball to arrive
        // 3. Retrieves the id of the waiting ball in b->first
        other = *b->first;
        // 4. Goes to the pointer stored in b->second, which points to the other ball's 'other' variable and update 'other' with second ball's id
        *b->second = id;
        // 5. Reset the barrier
        b->first = NULL;
        b->second = NULL;
        sem_post(&b->waiting);
        sem_post(&b->mutex);
    }
    // 6. First ball will have second ball's id saved in 'other', second ball will have first ball's id saved in 'other'
    return other;
}