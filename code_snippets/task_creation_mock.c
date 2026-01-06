#include <stdio.h>
#include <stdint.h>

/*
 * Phase 5: Task Creation API
 * 
 * Concept:
 * - Demonstrating the signature of xTaskCreate.
 * - Key Takeaway: Stack Depth is in WORDS (4 bytes), not Bytes.
 */

typedef void (*TaskFunction_t)(void *);

// Mock FreeRTOS function signature
void xTaskCreate(TaskFunction_t pxTaskCode,
                 const char * const pcName,
                 const uint16_t usStackDepth,
                 void * const pvParameters,
                 unsigned int uxPriority,
                 void ** const pxCreatedTask) {
    printf("\n[OS] xTaskCreate Called:\n");
    printf("  - Name: %s\n", pcName);
    printf("  - Stack: %d WORDS (Total Bytes: %d)\n", usStackDepth, usStackDepth * sizeof(int));
    printf("  - Priority: %d\n", uxPriority);
}

void my_task_code(void *params) {
    printf("Hello from Task!\n");
}

int main() {
    printf("=== Task Creation API Mock ===\n");
    
    // Create a task with 1024 WORDS of stack (4KB)
    xTaskCreate(my_task_code, "DemoTask", 1024, NULL, 2, NULL);

    return 0;
}
