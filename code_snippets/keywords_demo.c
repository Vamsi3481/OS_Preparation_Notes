#include <stdio.h>
#include <stdint.h>

/*
 * 1. volatile
 * Tells the compiler: "Do NOT optimize this variable. It can change at any time
 * (by hardware or ISR), even if this code doesn't change it."
 */
volatile int hardware_flag = 0; 

void wait_for_hardware() {
    printf("Waiting for hardware flag...\n");
    // Without 'volatile', the compiler might optimize this to: while(0) {} 
    // because it sees nobody changing 'hardware_flag' in this function.
    // With 'volatile', it re-reads the variable from memory EVERY time.
    int count = 0;
    while (hardware_flag == 0) {
        // Simulating hardware delay
        count++;
        if (count > 5) break; // Break just to not hang the demo
    }
    printf("Hardware flag check done.\n");
}

/*
 * 2. static
 * Inside a function: Persists value across calls.
 * Global scope: Limits visibility to THIS file only (Private).
 */
void counter_function() {
    static int count = 0; // Initialized only ONCE
    int local_var = 0;    // Re-initialized every call
    
    count++;
    local_var++;
    printf("Static: %d, Local: %d\n", count, local_var);
}

/*
 * 3. const
 * Read-only. Good for safety and placing data in Flash (Text segment) instead of RAM.
 */
void print_message(const char *msg) {
    // msg[0] = 'A'; // Compilation Error!
    printf("Message: %s\n", msg);
}

int main() {
    printf("=== 1. volatile Example ===\n");
    wait_for_hardware();

    printf("\n=== 2. static Example ===\n");
    counter_function();
    counter_function();
    counter_function();

    printf("\n=== 3. const Example ===\n");
    print_message("Hello Qualcomm!");

    return 0;
}
