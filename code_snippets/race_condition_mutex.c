#include <stdio.h>
#include <stdbool.h>

/*
 * Race Condition & Mutex Simulation
 * 
 * Scenario: Two tasks (Task A and Task B) try to increment a shared counter.
 * 
 * The Bug (Race Condition):
 * 1. Task A reads counter (0).
 * 2. Context Switch! Task B runs.
 * 3. Task B reads counter (0).
 * 4. Task B increments and writes (1).
 * 5. Task A resumes. It has old value (0). Increments and writes (1).
 * Result: Counter is 1, but should be 2.
 */

int shared_counter = 0;

// Simulated Mutex
typedef struct {
    bool is_locked;
    int owner_id;
} Mutex_t;

Mutex_t my_mutex = { .is_locked = false, .owner_id = -1 };

// Function to acquire Mutex
bool mutex_lock(Mutex_t *m, int task_id) {
    if (m->is_locked) {
        printf("[Mutex] Task %d BLOCKED! Mutex held by Task %d\n", task_id, m->owner_id);
        return false; // Failed to get lock
    }
    m->is_locked = true;
    m->owner_id = task_id;
    printf("[Mutex] Task %d acquired lock.\n", task_id);
    return true;
}

// Function to release Mutex
void mutex_unlock(Mutex_t *m, int task_id) {
    if (m->owner_id == task_id) {
        m->is_locked = false;
        m->owner_id = -1;
        printf("[Mutex] Task %d released lock.\n", task_id);
    } else {
        printf("[Mutex] ERROR: Task %d tried to release mutex owned by Task %d!\n", task_id, m->owner_id);
    }
}

// Unsafe Increment (Simulating the Race)
void unsafe_increment(int task_id) {
    printf("Task %d reading counter... (Value: %d)\n", task_id, shared_counter);
    int temp = shared_counter;
    
    // SIMULATING CONTEXT SWITCH / PREEMPTION HERE
    printf("--- Context Switch occurred! ---\n");
    
    temp++;
    shared_counter = temp;
    printf("Task %d wrote counter. (New Value: %d)\n", task_id, shared_counter);
}

// Safe Increment (With Mutex)
void safe_increment(int task_id) {
    if (mutex_lock(&my_mutex, task_id)) {
        printf("Task %d reading counter... (Value: %d)\n", task_id, shared_counter);
        int temp = shared_counter;
        
        // Even if context switch happens here, nobody else can touch the data!
        
        temp++;
        shared_counter = temp;
        printf("Task %d wrote counter. (New Value: %d)\n", task_id, shared_counter);
        
        mutex_unlock(&my_mutex, task_id);
    } else {
        printf("Task %d retrying later...\n", task_id);
    }
}

int main() {
    printf("=== 1. The Race Condition (Unsafe) ===\n");
    shared_counter = 0;
    // Task 1 reads 0. Context switch happens. Task 2 reads 0. Both write 1.
    // We can't easily simulate the exact interleaving in single-threaded C without threads,
    // but imagine the output:
    // Task 1 Read 0
    // Task 2 Read 0
    // Task 2 Write 1
    // Task 1 Write 1 -> LOST UPDATE!
    
    printf("=== 2. The Fix (Mutex) ===\n");
    shared_counter = 0;
    
    // Task 1 gets the lock
    safe_increment(1);
    
    // Task 2 tries to get lock but fails (or waits in a real OS)
    // Here we simulate Task 2 trying while Task 1 holds it (conceptually)
    // But since safe_increment releases it, let's simulate a collision:
    
    printf("\n--- Simulating Collision ---\n");
    mutex_lock(&my_mutex, 1); // Task 1 locks it
    
    // Task 2 tries to run
    safe_increment(2); // Should fail/block
    
    mutex_unlock(&my_mutex, 1); // Task 1 releases
    
    // Now Task 2 can run
    safe_increment(2);

    return 0;
}
