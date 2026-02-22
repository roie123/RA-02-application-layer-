//
// Created on 2/19/26.
//

#ifndef DV_QUEUE_EXAMPLE_H
#define DV_QUEUE_EXAMPLE_H

#include "DV_queue.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdint.h>

/**
 * @brief Example of using the static MPSC queue in a FreeRTOS project
 * 
 * This example shows how to define, initialize, and use a static MPSC queue
 * for communication between producer and consumer tasks.
 */

// Define a message structure
typedef struct {
    uint32_t id;
    uint32_t value;
    // Add more fields as needed
} message_t;

// Create a queue with 32 elements
static mpsc_queue_32_t message_queue;

// Example producer task
static void producer_task(void *pvParameters) {
    uint32_t producer_id = (uint32_t)pvParameters;
    uint32_t message_counter = 0;
    
    while (1) {
        // Create a static message (could be from a static pool in real app)
        static message_t messages[5];
        message_t *msg = &messages[message_counter % 5];
        
        // Fill the message with data
        msg->id = producer_id;
        msg->value = message_counter++;
        
        // Try to enqueue the message
        if (mpsc_queue_enqueue(&message_queue, msg)) {
            // Message was successfully enqueued
        } else {
            // Queue is full, handle the error
            // In a real application, you might want to wait or discard the message
        }
        
        // Delay before sending next message
        vTaskDelay(pdMS_TO_TICKS(100 + (producer_id * 50)));
    }
}

// Example consumer task
static void consumer_task(void *pvParameters) {
    while (1) {
        message_t *msg = NULL;
        
        // Try to dequeue a message
        if (mpsc_queue_dequeue(&message_queue, (void**)&msg)) {
            // Process the message
            printf("Received message from producer %lu with value %lu\n", 
                   (unsigned long)msg->id, (unsigned long)msg->value);
            
            // No need to free the message since it's from a static pool
        } else {
            // No message available, can yield or sleep
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

// Function to initialize the example
void queue_example_init(void) {
    // Initialize the queue
    mpsc_queue_init(&message_queue);
    
    // Create multiple producer tasks
    xTaskCreate(producer_task, "Producer1", configMINIMAL_STACK_SIZE, (void*)1, 2, NULL);
    xTaskCreate(producer_task, "Producer2", configMINIMAL_STACK_SIZE, (void*)2, 2, NULL);
    xTaskCreate(producer_task, "Producer3", configMINIMAL_STACK_SIZE, (void*)3, 2, NULL);
    
    // Create the consumer task
    xTaskCreate(consumer_task, "Consumer", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
}

/**
 * @brief Example of integrating the static MPSC queue with the FreeAct framework
 * 
 * This example shows how to modify the FreeAct framework to use the static MPSC queue.
 */

// Include these modifications in your FreeAct.h file:
/*
// Add this to FreeAct.h
#include "DV_queue.h"

// In the Active struct, replace the existing queue with:
struct Active {
    TaskHandle_t thread;     // private thread 
    StaticTask_t thread_cb;  // thread control-block (FreeRTOS static alloc)

    mpsc_queue_32_t event_queue;  // static MPSC queue for events
    
    DispatchHandler dispatch; // pointer to the dispatch() function
    
    // active object data added in subclasses of Active
};

// Then implement the queue operations in FreeAct.c
*/

// Example implementations for the Active functions in FreeAct.c:
/*
void Active_start(Active * const me,
                  uint8_t prio,
                  Event **queueSto,  // Not used, kept for compatibility
                  uint32_t queueLen,  // Not used, kept for compatibility
                  void *stackSto,
                  uint32_t stackSize,
                  TaskFunction_t task)
{
    StackType_t *stk_sto = stackSto;
    uint32_t stk_depth = (stackSize / sizeof(StackType_t));

    // Initialize the static MPSC queue
    mpsc_queue_init(&me->event_queue);

    me->thread = xTaskCreateStatic(
              task != NULL ? task : Active_eventLoop,
              "AO",
              stk_depth,
              me,
              prio + tskIDLE_PRIORITY,
              stk_sto,
              &me->thread_cb);

    configASSERT(me->thread);
}

void Active_post(Active * const me, Event const * const e) {
    // Create a static copy of the event
    static Event event_copies[32];
    static _Atomic uint32_t event_copy_index = 0;
    
    uint32_t index = atomic_fetch_add(&event_copy_index, 1) % 32;
    Event* e_copy = &event_copies[index];
    
    // Copy the event data
    *e_copy = *e;
    
    // Enqueue the event
    bool success = mpsc_queue_enqueue(&me->event_queue, e_copy);
    configASSERT(success);
}

void Active_postFromISR(Active * const me, Event const * const e,
                        BaseType_t *pxHigherPriorityTaskWoken)
{
    // Use the same mechanism as regular post since our queue is ISR-safe
    Active_post(me, e);
}

static void Active_eventLoop(void *pvParameters) {
    Active *me = (Active *)pvParameters;
    static Event const initEvt = { INIT_SIG };

    configASSERT(me);

    // Initialize the AO
    (*me->dispatch)(me, &initEvt);

    for (;;) {
        Event *e = NULL;

        // Try to dequeue an event
        if (mpsc_queue_dequeue(&me->event_queue, (void**)&e)) {
            configASSERT(e != NULL);
            
            // Dispatch event to the active object
            (*me->dispatch)(me, e);
            
            // No need to free the event as it's from our static pool
        } else {
            // No events available, yield or sleep
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
}
*/

#endif //DV_QUEUE_EXAMPLE_H
