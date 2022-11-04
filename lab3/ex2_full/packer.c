#include <semaphore.h>
#include <stddef.h>
#include <stdio.h>

#include "packer.h"

typedef struct {
    int *other_balls;
    int num_other_balls_arrived;
    int first_ball;
    sem_t mutex;
    sem_t waiting;
    sem_t first_ball_queue;
    sem_t wait_for_last;
    sem_t wait_for_saved_list;
} barrier;

barrier bar[3];
int n;
void remove_duplicate(int *source, int *dest, int n, int id);

void packer_init(int balls_per_pack) {
    n = balls_per_pack;
    for (int i = 0; i < 3; i++) {
        bar[i].other_balls = NULL;
        bar[i].num_other_balls_arrived = 0;
        sem_init(&bar[i].mutex, 0, 1);
        sem_init(&bar[i].first_ball_queue, 0, 1);
        sem_init(&bar[i].wait_for_last, 0, 0);
        sem_init(&bar[i].wait_for_saved_list, 0, 0);
    }
}

void packer_destroy(void) {
    for (int i = 0; i < 3; i++) {
        sem_destroy(&bar[i].mutex);
        sem_destroy(&bar[i].first_ball_queue);
        sem_destroy(&bar[i].wait_for_last);
        sem_destroy(&bar[i].wait_for_saved_list);
    }
}

void pack_ball(int colour, int id, int *other_ids) {
    barrier *b = &bar[colour - 1];
    sem_wait(&b->mutex);
    if (b->other_balls == NULL) { // First ball to arrive
        // printf("first ball %d, colour %d\n", id, colour);
        // 1. Ensure ordering of first ball of the same colour that arrives
        sem_wait(&b->first_ball_queue);
        b->other_balls = other_ids;
        // 2. Store the first ball's id (to remedy a bug where the first ball's id was not getting printed)
        b->first_ball = id;
        b->num_other_balls_arrived = 0;
        sem_post(&b->mutex);
        // 3. Wait for last ball to arrive
        sem_wait(&b->wait_for_last);
        // 8. Removes own id from the list
        remove_duplicate(b->other_balls, other_ids, n, id);
        // 9. Unlock wait_for_saved_list to indicate that the thread has saved the list
        sem_post(&b->wait_for_saved_list);
        // 10. Unlock first_ball_queue to let the next first ball of the same colour to proceed
        sem_post(&b->first_ball_queue);
    } else { // Other balls arrive
        // 4. Store id in b->other_balls and increment number of other balls that arrived
        b->other_balls[b->num_other_balls_arrived] = id;
        b->num_other_balls_arrived++;
        if (b->num_other_balls_arrived == n - 1) { // 6. Last ball arrives
            // printf("%d last ball\n", id);
            // 7. Unlock all wait_for_last semaphore
            for (int i = 0; i < n - 1; i++) {
                sem_post(&b->wait_for_last);
            }
            // 8. Removes own id from the list and add first ball's id into list
            remove_duplicate(b->other_balls, other_ids, n, id);
            other_ids[n - 2] = b->first_ball;
            // 9. Wait for all threads to save the list to their other_ids before resetting
            for (int i = 0; i < n - 1; i++) {
                sem_wait(&b->wait_for_saved_list);
            }
            b->other_balls = NULL;
            sem_post(&b->mutex);
        } else {
            // 5. Unlock mutex and wait for last ball to arrive
            // printf("%d other ball\n", id);
            sem_post(&b->mutex);
            sem_wait(&b->wait_for_last);
            // 8. Removes own id from the list and add the first ball's id into list
            remove_duplicate(b->other_balls, other_ids, n, id);
            other_ids[n - 2] = b->first_ball;
            // 9. Unlock wait_for_saved_list to indicate that the thread has saved the list
            sem_post(&b->wait_for_saved_list);
        }        
    }
}

void remove_duplicate(int *source, int *dest, int n, int id) {
    int k = 0;
    int l = 0;
    while (source[k] != id && k < n) {
        dest[l] = source[k];
        k++;
        l++;
    }
    k++;
    while(k < n) {
        dest[l] = source[k];
        l++;
        k++;
    }
}