#include <stdio.h>
#include <stdbool.h>

/*
 * Deferred Interrupt Processing (Top Half / Bottom Half)
 * 
 * The Problem: ISRs must be FAST. They block other interrupts.
 * The Solution: 
 * 1. Top Half (ISR): Do the minimum (clear flag, signal task).
 * 2. Bottom Half (Task): Do the heavy work (process data, print, etc).
 */

// Simulated Hardware Flag
volatile int uart_data_register = 0;
volatile bool interrupt_flag = false;

// Simulated OS Queue (Simple variable for demo)
int data_queue = 0;
bool queue_has_data = false;

// --- The Top Half (ISR) ---
// MUST be fast. No printf, no heavy math, no blocking.
void UART_ISR() {
    // 1. Read Data from Hardware
    int data = uart_data_register;
    
    // 2. Clear Interrupt Flag (Simulated)
    interrupt_flag = false;
    
    // 3. Send to Queue (Signal the Task)
    data_queue = data;
    queue_has_data = true;
    
    // 4. Request Context Switch (in real RTOS: portYIELD_FROM_ISR)
    // printf("ISR: Received %d. Signaled Task.\n", data); // BAD PRACTICE in real ISR!
}

// --- The Bottom Half (Task) ---
// Can take its time. Can block.
void UART_Processing_Task() {
    if (queue_has_data) {
        // 1. Read from Queue
        int data = data_queue;
        queue_has_data = false;
        
        // 2. Process Data (Heavy work)
        printf("Task: Processing data %d... ", data);
        printf("[Complex Math]... ");
        printf("Done.\n");
    } else {
        printf("Task: Waiting for data...\n");
    }
}

int main() {
    printf("--- System Start ---\n");
    
    // 1. Simulate Hardware Event
    printf("\n[Hardware]: Data 0xAA arrives!\n");
    uart_data_register = 0xAA;
    interrupt_flag = true;
    
    // 2. Interrupt Fires!
    if (interrupt_flag) {
        UART_ISR();
    }
    
    // 3. Scheduler runs the Task
    UART_Processing_Task();
    
    // 4. Simulate another event
    printf("\n[Hardware]: Data 0xBB arrives!\n");
    uart_data_register = 0xBB;
    interrupt_flag = true;
    
    if (interrupt_flag) {
        UART_ISR();
    }
    
    UART_Processing_Task();

    return 0;
}
