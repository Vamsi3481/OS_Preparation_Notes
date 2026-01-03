#include <stdio.h>
#include <stdlib.h>

/* 
 * 1. void* (Generic Pointer)
 * Used heavily in FreeRTOS for passing data to Tasks/Queues.
 * It can point to ANYTHING, but you must cast it back to use it.
 */
void print_data(void *data, int type) {
    if (type == 1) {
        // Cast to int* then dereference
        printf("Int Data: %d\n", *(int*)data);
    } else if (type == 2) {
        // Cast to char* then dereference
        printf("Char Data: %c\n", *(char*)data);
    }
}

/*
 * 2. Function Pointers
 * Used for Callbacks and Interrupt Handlers.
 * Syntax: return_type (*name)(arguments)
 */
void error_handler(const char *msg) {
    printf("ERROR: %s\n", msg);
}

void success_handler(const char *msg) {
    printf("SUCCESS: %s\n", msg);
}

// A function that takes a function pointer as an argument
void perform_operation(int a, int b, void (*callback)(const char*)) {
    if (b == 0) {
        callback("Division by zero!");
    } else {
        printf("Result: %d\n", a / b);
        callback("Operation complete.");
    }
}

/*
 * 3. Double Pointers (int**)
 * Used when you need to change WHERE a pointer points.
 * Common in Linked Lists (modifying the 'head' pointer).
 */
void allocate_memory(int **ptr) {
    // We receive the ADDRESS of the pointer 'p' from main.
    // We dereference it once to get to 'p', and change 'p' to point to new memory.
    *ptr = (int*)malloc(sizeof(int));
    **ptr = 99; // Write 99 to that new memory
}

int main() {
    printf("=== 1. void* Example ===\n");
    int x = 42;
    char c = 'Z';
    print_data(&x, 1);
    print_data(&c, 2);

    printf("\n=== 2. Function Pointer Example ===\n");
    perform_operation(10, 2, success_handler);
    perform_operation(10, 0, error_handler);

    printf("\n=== 3. Double Pointer Example ===\n");
    int *p = NULL;
    printf("Before: p is %p\n", (void*)p);
    
    // Pass the ADDRESS of 'p' (&p) so the function can modify 'p'
    allocate_memory(&p);
    
    printf("After : p is %p, value is %d\n", (void*)p, *p);
    
    free(p);
    return 0;
}
