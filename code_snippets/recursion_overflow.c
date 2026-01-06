#include <stdio.h>

/*
 * Phase 1: Recursion & Stack Overflow
 * 
 * Concept:
 * - Each function call pushes a "Stack Frame" (Return Address, Arguments, Locals).
 * - Infinite Recursion = Infinite Stack Frames.
 * - Eventually, the Stack Pointer (SP) hits the limit -> Stack Overflow.
 */

void recursive_function(int depth) {
    int local_var = depth; // Takes up stack space
    printf("Recursion Depth: %d (Stack Address: %p)\n", depth, &local_var);
    
    // Safety limit to prevent actual crash during demo
    if (depth < 5) {
        recursive_function(depth + 1);
    }
    // If we removed the 'if', this would crash the program (Stack Overflow).
}

int main() {
    printf("=== Recursion & Stack Overflow Demo ===\n");
    recursive_function(1);
    return 0;
}
