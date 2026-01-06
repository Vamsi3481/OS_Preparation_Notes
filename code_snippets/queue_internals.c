#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/*
 * Phase 5: Queue Internals (Deep Dive)
 * 
 * What happens INSIDE the Queue when you block?
 * The Queue struct contains TWO Linked Lists:
 * 1. xTasksWaitingToSend: Tasks waiting for space to write.
 * 2. xTasksWaitingToReceive: Tasks waiting for data to read.
 * 
 * When you block, you are moved from the Ready List to one of these lists.
 */

// Simulated TCB
typedef struct {
    int id;
    const char *name;
    bool is_blocked;
} TCB_t;

// Simulated List
typedef struct {
    TCB_t *tasks[5];
    int count;
} List_t;

// Simulated Queue
typedef struct {
    int buffer[2]; // Small buffer (size 2)
    int count;
    int size;
    List_t xTasksWaitingToSend;    // Blocked Writers
    List_t xTasksWaitingToReceive; // Blocked Readers
} Queue_t;

void list_add(List_t *list, TCB_t *task) {
    list->tasks[list->count++] = task;
    task->is_blocked = true;
    printf("[OS] Task '%s' BLOCKED and added to Queue Wait List.\n", task->name);
}

TCB_t* list_remove_first(List_t *list) {
    if (list->count == 0) return NULL;
    TCB_t *task = list->tasks[0];
    // Shift remaining (Simplified)
    for(int i=0; i<list->count-1; i++) list->tasks[i] = list->tasks[i+1];
    list->count--;
    task->is_blocked = false;
    printf("[OS] Task '%s' UNBLOCKED (Moved to Ready List).\n", task->name);
    return task;
}

void queue_send(Queue_t *q, int data, TCB_t *task) {
    if (q->count < q->size) {
        q->buffer[q->count++] = data;
        printf("[Queue] '%s' sent %d. (Count: %d/%d)\n", task->name, data, q->count, q->size);
        
        // Check if anyone was waiting to receive
        if (q->xTasksWaitingToReceive.count > 0) {
            list_remove_first(&q->xTasksWaitingToReceive);
        }
    } else {
        printf("[Queue] FULL! '%s' wants to send.\n", task->name);
        list_add(&q->xTasksWaitingToSend, task);
    }
}

void queue_receive(Queue_t *q, TCB_t *task) {
    if (q->count > 0) {
        int data = q->buffer[0]; // Simplified FIFO
        q->count--;
        printf("[Queue] '%s' received %d. (Count: %d/%d)\n", task->name, data, q->count, q->size);
        
        // Check if anyone was waiting to send
        if (q->xTasksWaitingToSend.count > 0) {
            TCB_t *writer = list_remove_first(&q->xTasksWaitingToSend);
            // In real OS, we might let the writer write immediately or yield
            printf("[OS] Resuming writer '%s'...\n", writer->name);
            queue_send(q, 999, writer); // Retry send
        }
    } else {
        printf("[Queue] EMPTY! '%s' wants to receive.\n", task->name);
        list_add(&q->xTasksWaitingToReceive, task);
    }
}

int main() {
    printf("=== FreeRTOS Queue Internals Simulation ===\n");
    
    Queue_t q = { .size = 2, .count = 0, .xTasksWaitingToSend = {0}, .xTasksWaitingToReceive = {0} };
    
    TCB_t t1 = {1, "Writer1", false};
    TCB_t t2 = {2, "Writer2", false};
    TCB_t t3 = {3, "Writer3", false};
    TCB_t reader = {4, "Reader", false};
    
    // 1. Fill the Queue
    queue_send(&q, 10, &t1);
    queue_send(&q, 20, &t2);
    
    // 2. Overflow -> Block Writer3
    queue_send(&q, 30, &t3); // Should block
    
    // 3. Reader reads one item
    printf("\n--- Reader Arrives ---\n");
    queue_receive(&q, &reader); // Should unblock Writer3
    
    return 0;
}
