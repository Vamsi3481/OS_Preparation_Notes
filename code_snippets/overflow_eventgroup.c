#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/*
 * Phase 4 Missing Topics:
 * 1. Stack Overflow Detection (The Canary Method)
 * 2. Event Groups (Bitwise Flags)
 */

// --- Topic 1: Stack Overflow Detection ---
#define STACK_SIZE 10
#define CANARY_VALUE 0xDEADBEEF

typedef struct {
    uint32_t stack[STACK_SIZE];
    int top;
} TaskStack_t;

void init_stack(TaskStack_t *t) {
    // Fill the stack with a known pattern ("Painting the Stack")
    for(int i=0; i<STACK_SIZE; i++) {
        t->stack[i] = CANARY_VALUE;
    }
    t->top = STACK_SIZE; // Stack grows down
}

// Simulate pushing data (Function calls)
void stack_push(TaskStack_t *t, uint32_t val) {
    if (t->top > 0) {
        t->top--;
        t->stack[t->top] = val;
    } else {
        // OVERFLOW! We are writing outside the bounds (conceptually)
        // In this sim, we just print error, but in real HW, this corrupts memory.
        printf("[CRITICAL] Stack Overflow! Writing to forbidden memory.\n");
    }
}

// Check if the stack has overflowed (FreeRTOS Method 2)
bool check_overflow(TaskStack_t *t) {
    // Check the LAST word of the stack (index 0 in our array, since it grows down to 0)
    // If it's no longer the Canary value, we overflowed (or got very close).
    if (t->stack[0] != CANARY_VALUE) {
        return true; // Overflow detected!
    }
    return false;
}

// --- Topic 2: Event Groups ---
// Scenario: Task waits for WiFi (Bit 0) AND Bluetooth (Bit 1) to be ready.
#define BIT_WIFI  (1 << 0) // 0x01
#define BIT_BLE   (1 << 1) // 0x02

uint32_t event_group = 0;

void set_event_bit(uint32_t bit) {
    event_group |= bit; // OR operation to set bit
    printf("[EventGroup] Bit set. Current Value: 0x%X\n", event_group);
}

void wait_for_all_events() {
    uint32_t target = BIT_WIFI | BIT_BLE; // 0x03
    
    printf("[Task] Waiting for WiFi (0x1) AND BLE (0x2)...\n");
    
    if ((event_group & target) == target) {
        printf("[Task] SUCCESS! Both events happened.\n");
    } else {
        printf("[Task] Still waiting... (Current: 0x%X)\n", event_group);
    }
}

int main() {
    printf("=== 1. Stack Overflow Detection (Canary) ===\n");
    TaskStack_t my_stack;
    init_stack(&my_stack);
    
    // Push some values (Safe)
    stack_push(&my_stack, 1);
    stack_push(&my_stack, 2);
    
    if (check_overflow(&my_stack)) printf("Overflow? YES\n");
    else printf("Overflow? NO (Canary intact: 0x%X)\n", my_stack.stack[0]);
    
    // Push too many values (Unsafe)
    printf("Pushing until overflow...\n");
    for(int i=0; i<10; i++) stack_push(&my_stack, i+10);
    
    // Simulate overwriting the bottom-most slot (The Canary)
    // In real life, the stack pointer would go below 0.
    // Here we manually corrupt the canary to simulate the effect.
    my_stack.stack[0] = 0x12345678; 
    
    if (check_overflow(&my_stack)) printf("Overflow? YES (Canary died! Found: 0x%X)\n", my_stack.stack[0]);

    printf("\n=== 2. Event Groups (Bitwise Logic) ===\n");
    event_group = 0;
    wait_for_all_events();
    
    printf("-> WiFi connects...\n");
    set_event_bit(BIT_WIFI);
    wait_for_all_events();
    
    printf("-> BLE connects...\n");
    set_event_bit(BIT_BLE);
    wait_for_all_events();

    return 0;
}
