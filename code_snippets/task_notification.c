#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Phase 4 Missing Topic: Task Notifications vs Semaphores
 * 
 * Scenario: An ISR needs to wake up a Task.
 * Method 1: Binary Semaphore (Standard, heavier).
 * Method 2: Task Notification (Faster, lighter).
 */

// --- Method 1: Binary Semaphore ---
typedef struct {
    bool token;
} Semaphore_t;

Semaphore_t my_sem = { .token = false };

void ISR_Give_Semaphore() {
    my_sem.token = true;
    printf("[ISR] Gave Semaphore.\n");
}

void Task_Take_Semaphore() {
    if (my_sem.token) {
        my_sem.token = false;
        printf("[Task] Took Semaphore. Processing...\n");
    } else {
        printf("[Task] Blocked waiting for Semaphore.\n");
    }
}

// --- Method 2: Task Notification ---
// Inside the TCB, there is ALREADY a 32-bit value.
typedef struct {
    uint32_t ulNotifiedValue;
    uint8_t ucNotifyState; // 0=NotWaiting, 1=Waiting, 2=Notified
} TCB_Stub_t;

TCB_Stub_t my_task_tcb = { .ulNotifiedValue = 0, .ucNotifyState = 0 };

void ISR_Notify_Task() {
    // Direct to Task! No intermediate object.
    my_task_tcb.ulNotifiedValue++; // Can be used as a counter!
    my_task_tcb.ucNotifyState = 2; // Set to "Notified"
    printf("[ISR] Sent Notification to Task TCB. (Value: %d)\n", my_task_tcb.ulNotifiedValue);
}

void Task_Wait_Notification() {
    if (my_task_tcb.ucNotifyState == 2) {
        uint32_t val = my_task_tcb.ulNotifiedValue;
        my_task_tcb.ucNotifyState = 0; // Clear state
        // Optional: Clear value or keep it (Counting Semaphore behavior)
        my_task_tcb.ulNotifiedValue = 0; 
        printf("[Task] Received Notification! Value was: %d\n", val);
    } else {
        printf("[Task] Blocked waiting for Notification.\n");
    }
}

int main() {
    printf("=== 1. Binary Semaphore (Object-based) ===\n");
    Task_Take_Semaphore(); // Fails
    ISR_Give_Semaphore();
    Task_Take_Semaphore(); // Succeeds

    printf("\n=== 2. Task Notification (Direct-to-Task) ===\n");
    Task_Wait_Notification(); // Fails
    ISR_Notify_Task();
    ISR_Notify_Task(); // Note: It counts! (Value becomes 2)
    Task_Wait_Notification(); // Succeeds

    return 0;
}
