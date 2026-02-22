//
// Created by royivri on 2/19/26.
//

#ifndef DV_QUEUE_H
#define DV_QUEUE_H

#include <stddef.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Node structure for the queue
 * 
 * This is a non-intrusive implementation, meaning the data is stored
 * separately from the node structure.
 */
typedef struct mpsc_node {
    void* data;
    _Atomic(struct mpsc_node*) next;
} mpsc_node_t;

/**
 * @brief Multiple Producer Single Consumer Queue
 * 
 * This is an implementation of Dmitry Vyukov's MPSC queue with static allocation.
 * It allows multiple threads to safely enqueue items while a single thread dequeues.
 * 
 * @param CAPACITY The maximum number of elements the queue can hold
 */
#define MPSC_QUEUE_DEFINE(CAPACITY) \
    typedef struct { \
        _Atomic(mpsc_node_t*) head; \
        mpsc_node_t* tail; \
        mpsc_node_t stub; \
        mpsc_node_t nodes[CAPACITY]; \
        _Atomic uint32_t free_nodes; \
        uint32_t capacity; \
    } mpsc_queue_##CAPACITY##_t

// Define a few common queue sizes
MPSC_QUEUE_DEFINE(8);
MPSC_QUEUE_DEFINE(16);
MPSC_QUEUE_DEFINE(32);
MPSC_QUEUE_DEFINE(64);
MPSC_QUEUE_DEFINE(128);

/**
 * @brief Get a free node from the queue's pre-allocated pool
 * 
 * @param queue Pointer to the queue
 * @return mpsc_node_t* Pointer to a free node, or NULL if none available
 */
#define mpsc_queue_get_free_node(queue_ptr, queue_type) _Generic((queue_ptr), \
    mpsc_queue_8_t*: mpsc_queue_get_free_node_impl((queue_ptr), 8), \
    mpsc_queue_16_t*: mpsc_queue_get_free_node_impl((queue_ptr), 16), \
    mpsc_queue_32_t*: mpsc_queue_get_free_node_impl((queue_ptr), 32), \
    mpsc_queue_64_t*: mpsc_queue_get_free_node_impl((queue_ptr), 64), \
    mpsc_queue_128_t*: mpsc_queue_get_free_node_impl((queue_ptr), 128) \
)

/**
 * @brief Check if the queue has free nodes available
 * 
 * @param queue_ptr Pointer to the queue
 * @return true if the queue has free nodes, false otherwise
 */
#define mpsc_queue_has_free_nodes(queue_ptr) _Generic((queue_ptr), \
    mpsc_queue_8_t*: mpsc_queue_has_free_nodes_impl((queue_ptr)), \
    mpsc_queue_16_t*: mpsc_queue_has_free_nodes_impl((queue_ptr)), \
    mpsc_queue_32_t*: mpsc_queue_has_free_nodes_impl((queue_ptr)), \
    mpsc_queue_64_t*: mpsc_queue_has_free_nodes_impl((queue_ptr)), \
    mpsc_queue_128_t*: mpsc_queue_has_free_nodes_impl((queue_ptr)) \
)

/**
 * @brief Initialize a MPSC queue
 * 
 * @param queue_ptr Pointer to the queue
 * @return true if initialization succeeded
 * @return false if initialization failed
 */
#define mpsc_queue_init(queue_ptr) _Generic((queue_ptr), \
    mpsc_queue_8_t*: mpsc_queue_init_impl((queue_ptr), 8), \
    mpsc_queue_16_t*: mpsc_queue_init_impl((queue_ptr), 16), \
    mpsc_queue_32_t*: mpsc_queue_init_impl((queue_ptr), 32), \
    mpsc_queue_64_t*: mpsc_queue_init_impl((queue_ptr), 64), \
    mpsc_queue_128_t*: mpsc_queue_init_impl((queue_ptr), 128) \
)

/**
 * @brief Enqueue an item (producer operation)
 * 
 * @param queue_ptr Pointer to the queue
 * @param data The data to enqueue
 * @return true if enqueue succeeded
 * @return false if enqueue failed (e.g., queue is full)
 */
#define mpsc_queue_enqueue(queue_ptr, data_ptr) _Generic((queue_ptr), \
    mpsc_queue_8_t*: mpsc_queue_enqueue_impl((queue_ptr), (data_ptr)), \
    mpsc_queue_16_t*: mpsc_queue_enqueue_impl((queue_ptr), (data_ptr)), \
    mpsc_queue_32_t*: mpsc_queue_enqueue_impl((queue_ptr), (data_ptr)), \
    mpsc_queue_64_t*: mpsc_queue_enqueue_impl((queue_ptr), (data_ptr)), \
    mpsc_queue_128_t*: mpsc_queue_enqueue_impl((queue_ptr), (data_ptr)) \
)

/**
 * @brief Dequeue an item (consumer operation)
 * 
 * @param queue_ptr Pointer to the queue
 * @param data_out Pointer to where the dequeued data should be stored
 * @return true if dequeue succeeded (data was available)
 * @return false if the queue was empty
 */
#define mpsc_queue_dequeue(queue_ptr, data_out_ptr) _Generic((queue_ptr), \
    mpsc_queue_8_t*: mpsc_queue_dequeue_impl((queue_ptr), (data_out_ptr)), \
    mpsc_queue_16_t*: mpsc_queue_dequeue_impl((queue_ptr), (data_out_ptr)), \
    mpsc_queue_32_t*: mpsc_queue_dequeue_impl((queue_ptr), (data_out_ptr)), \
    mpsc_queue_64_t*: mpsc_queue_dequeue_impl((queue_ptr), (data_out_ptr)), \
    mpsc_queue_128_t*: mpsc_queue_dequeue_impl((queue_ptr), (data_out_ptr)) \
)

/**
 * @brief Check if the queue is empty
 * 
 * @param queue_ptr Pointer to the queue
 * @return true if the queue is empty
 * @return false if the queue is not empty or an error occurred
 */
#define mpsc_queue_is_empty(queue_ptr) _Generic((queue_ptr), \
    mpsc_queue_8_t*: mpsc_queue_is_empty_impl((queue_ptr)), \
    mpsc_queue_16_t*: mpsc_queue_is_empty_impl((queue_ptr)), \
    mpsc_queue_32_t*: mpsc_queue_is_empty_impl((queue_ptr)), \
    mpsc_queue_64_t*: mpsc_queue_is_empty_impl((queue_ptr)), \
    mpsc_queue_128_t*: mpsc_queue_is_empty_impl((queue_ptr)) \
)

/* Implementation functions - don't call these directly, use the macros above */

static inline mpsc_node_t* mpsc_queue_get_free_node_impl(void* queue, uint32_t capacity) {
    uint8_t* queue_bytes = (uint8_t*)queue;
    mpsc_node_t* nodes = (mpsc_node_t*)(queue_bytes + offsetof(mpsc_queue_8_t, nodes));
    _Atomic uint32_t* free_nodes = (_Atomic uint32_t*)(queue_bytes + offsetof(mpsc_queue_8_t, free_nodes));
    
    uint32_t current_free = atomic_load_explicit(free_nodes, memory_order_acquire);
    if (current_free == 0) {
        return NULL; // No free nodes available
    }
    
    uint32_t next_free;
    do {
        next_free = current_free - 1;
        // Try to decrement free_nodes atomically
    } while (!atomic_compare_exchange_weak_explicit(
                free_nodes,
                &current_free,
                next_free,
                memory_order_acq_rel,
                memory_order_acquire));
    
    // Calculate which node to use - this distributes them evenly
    uint32_t node_index = next_free % capacity;
    return &nodes[node_index];
}

static inline bool mpsc_queue_has_free_nodes_impl(void* queue) {
    uint8_t* queue_bytes = (uint8_t*)queue;
    _Atomic uint32_t* free_nodes = (_Atomic uint32_t*)(queue_bytes + offsetof(mpsc_queue_8_t, free_nodes));
    
    return atomic_load_explicit(free_nodes, memory_order_acquire) > 0;
}

static inline bool mpsc_queue_init_impl(void* queue, uint32_t capacity) {
    if (queue == NULL) {
        return false;
    }
    
    uint8_t* queue_bytes = (uint8_t*)queue;
    mpsc_node_t* stub = (mpsc_node_t*)(queue_bytes + offsetof(mpsc_queue_8_t, stub));
    _Atomic(mpsc_node_t*)* head = (_Atomic(mpsc_node_t*)*)(queue_bytes + offsetof(mpsc_queue_8_t, head));
    mpsc_node_t** tail = (mpsc_node_t**)(queue_bytes + offsetof(mpsc_queue_8_t, tail));
    _Atomic uint32_t* free_nodes = (_Atomic uint32_t*)(queue_bytes + offsetof(mpsc_queue_8_t, free_nodes));
    uint32_t* capacity_ptr = (uint32_t*)(queue_bytes + offsetof(mpsc_queue_8_t, capacity));
    
    stub->data = NULL;
    atomic_store_explicit(&stub->next, NULL, memory_order_relaxed);
    
    atomic_store_explicit(head, stub, memory_order_relaxed);
    *tail = stub;
    
    atomic_store_explicit(free_nodes, capacity, memory_order_relaxed);
    *capacity_ptr = capacity;
    
    // Initialize all nodes in the pool
    mpsc_node_t* nodes = (mpsc_node_t*)(queue_bytes + offsetof(mpsc_queue_8_t, nodes));
    for (uint32_t i = 0; i < capacity; i++) {
        nodes[i].data = NULL;
        atomic_store_explicit(&nodes[i].next, NULL, memory_order_relaxed);
    }
    
    return true;
}

static inline bool mpsc_queue_enqueue_impl(void* queue, void* data) {
    if (queue == NULL) {
        return false;
    }
    
    uint8_t* queue_bytes = (uint8_t*)queue;
    _Atomic(mpsc_node_t*)* head = (_Atomic(mpsc_node_t*)*)(queue_bytes + offsetof(mpsc_queue_8_t, head));
    
    // Get a pre-allocated node from the pool
    mpsc_node_t* node = mpsc_queue_get_free_node_impl(queue, *((uint32_t*)(queue_bytes + offsetof(mpsc_queue_8_t, capacity))));
    if (node == NULL) {
        return false; // Queue is full
    }
    
    node->data = data;
    atomic_store_explicit(&node->next, NULL, memory_order_relaxed);
    
    // Exchange the current head with our new node using atomic exchange
    mpsc_node_t* prev = atomic_exchange_explicit(head, node, memory_order_acq_rel);
    
    // Link the previous head to our new node
    atomic_store_explicit(&prev->next, node, memory_order_release);
    
    return true;
}

static inline bool mpsc_queue_dequeue_impl(void* queue, void** data_out) {
    if (queue == NULL || data_out == NULL) {
        return false;
    }
    
    uint8_t* queue_bytes = (uint8_t*)queue;
    mpsc_node_t** tail = (mpsc_node_t**)(queue_bytes + offsetof(mpsc_queue_8_t, tail));
    mpsc_node_t* stub = (mpsc_node_t*)(queue_bytes + offsetof(mpsc_queue_8_t, stub));
    _Atomic uint32_t* free_nodes = (_Atomic uint32_t*)(queue_bytes + offsetof(mpsc_queue_8_t, free_nodes));
    
    // Load the next node from the tail
    mpsc_node_t* tail_node = *tail;
    mpsc_node_t* next = atomic_load_explicit(&tail_node->next, memory_order_acquire);
    
    // If next is NULL, the queue is empty
    if (next == NULL) {
        return false;
    }
    
    // Move tail to the next node
    *tail = next;
    *data_out = next->data;
    
    // Mark the previous tail node as free (available for reuse)
    if (tail_node != stub) {
        tail_node->data = NULL;
        atomic_store_explicit(&tail_node->next, NULL, memory_order_relaxed);
        atomic_fetch_add_explicit(free_nodes, 1, memory_order_release);
    } else {
        // If we've dequeued from the stub, recycle it by making it the new head
        tail_node->data = NULL;
        atomic_store_explicit(&tail_node->next, NULL, memory_order_relaxed);
        
        _Atomic(mpsc_node_t*)* head = (_Atomic(mpsc_node_t*)*)(queue_bytes + offsetof(mpsc_queue_8_t, head));
        mpsc_node_t* prev = atomic_exchange_explicit(head, tail_node, memory_order_acq_rel);
        atomic_store_explicit(&prev->next, tail_node, memory_order_release);
    }
    
    return true;
}

static inline bool mpsc_queue_is_empty_impl(void* queue) {
    if (queue == NULL) {
        return true;
    }
    
    uint8_t* queue_bytes = (uint8_t*)queue;
    mpsc_node_t** tail = (mpsc_node_t**)(queue_bytes + offsetof(mpsc_queue_8_t, tail));
    
    return atomic_load_explicit(&(*tail)->next, memory_order_acquire) == NULL;
}

/**
 * @brief Check if the queue is full
 * 
 * @param queue_ptr Pointer to the queue
 * @return true if the queue is full
 * @return false if the queue has space available
 */
#define mpsc_queue_is_full(queue_ptr) (!mpsc_queue_has_free_nodes(queue_ptr))

#endif //DV_QUEUE_H
