#include "opt-synchprobs.h"
#include <types.h>  /* required by lib.h */
#include <lib.h>    /* for kprintf */
#include <synch.h>  /* for P(), V(), sem_* */
#include <thread.h> /* for thread_fork() */
#include <test.h>

#include "dining_driver.h"

// Left neighbour of current philosopher
// + NUM_PHILOSOPHER to prevent modulo on a negative number (if phil_num was 0)
#define LEFT ((phil_num + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS)

// Right neighbour of current philosopher
#define RIGHT ((phil_num + 1) % NUM_PHILOSOPHERS)

// Potential states of the philosophers
#define HUNGRY 0
#define EATING 1
#define THINKING 2

/*
 * Declare any data structures you might need to synchronise 
 * your forks here.
 */

// Array to store the state of the philosophers
int state[NUM_PHILOSOPHERS];

// Semaphore to ensure mutually exclusive access to some critical sections
struct semaphore *mutex;

// Each philosopher gets their own semaphore (to allow blocking themselves when they don't have access to enough forks)
struct semaphore *s[NUM_PHILOSOPHERS];

/*
 * Take forks ensures mutually exclusive access to two forks
 * associated with the philosopher.
 * 
 * The left fork number = phil_num
 * The right fork number = (phil_num + 1) % NUM_PHILOSPHERS
 */

// Prototype of check_forks
void check_forks(unsigned long);

void take_forks(unsigned long phil_num)
{
    // Ensure mutual exclusion of critical section
    P(mutex);
    // Set current philosophers state to hungry (ready to eat)
    state[phil_num] = HUNGRY;
    // Check if there are free forks next to me, if there are grab them
    check_forks(phil_num);
    V(mutex);
    // Philosopher blocks himself while waiting for fork to become available
    P(s[phil_num]);
}


/*
 * Put forks releases the mutually exclusive access to the
 * philosophers forks.
 */

void put_forks(unsigned long phil_num)
{
    // Ensure mutual exclusion of critical section
    P(mutex);
    // Set current philosophers state to thinking (not wanting to eat)
    state[phil_num] = THINKING;
    // Unblock left and right neighbours if possible and let them start eating (hand them the respective forks)
    check_forks(LEFT);
    check_forks(RIGHT);
    V(mutex);
}


/* 
 * Create gets called before the philosopher threads get started.
 * Insert any initialisation code you require here.
 */

void create_forks()
{
    mutex = sem_create("mutex", 1);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        s[i] = sem_create("s", 0);
    }
}


/*
 * Destroy gets called when the system is shutting down.
 * You should clean up whatever you allocated in create_forks()
 */

void destroy_forks()
{
    sem_destroy(mutex);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++){
        sem_destroy(s[i]);
    }
}


void check_forks(unsigned long phil_num)
{
    // If current philosopher is hungry and left and right neighbours are not using their forks, grab them
    if (state[phil_num] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[phil_num] = EATING;
        V(s[phil_num]);
    }
}