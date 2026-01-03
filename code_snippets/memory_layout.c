#include <stdio.h>
#include <stdlib.h>

int global_var = 10; // Data Segment (Initialized global)
int bss_var;         // BSS Segment (Uninitialized global)

void foo(int call_depth) {
    int stack_var;
    printf("Stack Frame Depth %d: Address of stack_var: %p\n", call_depth, (void*)&stack_var);
    
    if (call_depth < 3) {
        foo(call_depth + 1);
    }
}

int main() {
    int stack_var_main = 20; // Stack
    int *heap_var = (int*)malloc(sizeof(int)); // Heap

    printf("=== Memory Addresses ===\n");
    printf("Text Segment (Code) : %p\n", (void*)main);
    printf("Data Segment (Global): %p\n", (void*)&global_var);
    printf("BSS Segment (Global) : %p\n", (void*)&bss_var);
    printf("Heap Segment         : %p\n", (void*)heap_var);
    printf("Stack (Main)         : %p\n", (void*)&stack_var_main);
    
    printf("\n=== Stack Growth ===\n");
    foo(1);

    free(heap_var);
    return 0;
}
