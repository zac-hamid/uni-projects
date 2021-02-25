/* This file will contain your solution. Modify it as you wish. */
#include <types.h>
#include <lib.h>
#include <synch.h>
#include "producerconsumer_driver.h"

/* Declare any variables you need here to keep track of and
   synchronise your bounded. A sample declaration of a buffer is shown
   below. It is an array of pointers to items.
   
   You can change this if you choose another implementation. 
   However, you should not have a buffer bigger than BUFFER_SIZE 
*/

data_item_t * item_buffer[BUFFER_SIZE];
int head; // Denotes index of the item that will leave the queue first
struct lock * buffer_lock;
struct cv *full;
struct cv *empty;
int num_queue_items = 0;

bool empty_slot = false; // Shouldn't be needed, just precautionary!

/* consumer_receive() is called by a consumer to request more data. It
   should block on a sync primitive if no data is available in your
   buffer. */

data_item_t * consumer_receive(void)
{
        data_item_t * item;

        // Ensure mutual exclusion into critical section
        lock_acquire(buffer_lock);
        // If buffer is empty, block this thread, nothing left to do for now
        while(num_queue_items == 0) cv_wait(empty,buffer_lock);
        // Remove item from queue
        item = item_buffer[head];
        item_buffer[head] = NULL;
        // Update head of buffer to be next item
        head = (head + 1) % BUFFER_SIZE;
        num_queue_items--;
        // Signal one of the producer threads to wake up since there is now space in the buffer
        cv_signal(full, buffer_lock);
        lock_release(buffer_lock);

        return item;
}

/* procucer_send() is called by a producer to store data in your
   bounded buffer. */

void producer_send(data_item_t *item)
{
        // Ensure mutual exclusion into critical section
        lock_acquire(buffer_lock);
        // If buffer is full, block this thread
        while (num_queue_items == BUFFER_SIZE) cv_wait(full, buffer_lock);
        // Insert_item into queue
        for (int i = 0; i < BUFFER_SIZE; i++) {
                // Check through each slot in list starting from the head of the list, is it free? (ie = NULL)
                if (item_buffer[(head + i) % BUFFER_SIZE] == NULL) {
                        // Found the next empty slot!
                        // Insert item into the buffer
                        item_buffer[((head + i) % BUFFER_SIZE)] = item;
                        empty_slot = true;
                        break;
                }
        }

        // this panic shouldn't be called unless something is very wrong
        // since number of items in buffer was already checked before finding a free slot
        if (!empty_slot) panic("Buffer overflow!\n");

        num_queue_items++;
        // Signal a consumer thread to wake up since there is now something new to take out of the buffer
        cv_signal(empty,buffer_lock);
        lock_release(buffer_lock);
}




/* Perform any initialisation (e.g. of global data) you need
   here. Note: You can panic if any allocation fails during setup */

void producerconsumer_startup(void)
{
        // Create lock and condition variables
        buffer_lock = lock_create("lock");
        full = cv_create("full");
        empty = cv_create("empty");
}

/* Perform any clean-up you need here */
void producerconsumer_shutdown(void)
{
        // Clean up lock and condition variables
        lock_destroy(buffer_lock);
        cv_destroy(full);
        cv_destroy(empty);
}

