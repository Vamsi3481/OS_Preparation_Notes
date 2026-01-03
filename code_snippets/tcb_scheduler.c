#include <stdio.h>
#include <stdint.h>
#include <string.h>

/*
 * RTOS Core: The Task Control Block (TCB)
 * 
 * This struct IS the task. It holds everything the OS needs to know.
 * When we "switch tasks", we are just switching which TCB we are looking at.
 */

// 1. The Stack
// Each task needs its own stack.
#define STACK_SIZE 128
typedef uint32_t StackType_t;

// 2. The TCB
typedef struct TCB {
    volatile StackType_t *pxTopOfStack; // Pointer to the last item pushed
    char pcTaskName[16];                // Name for debugging
    int uxPriority;                     // Priority (0 = Low)
    StackType_t pxStack[STACK_SIZE];    // The actual stack memory
} TCB_t;

// Global pointer to the Currently Running Task
TCB_t *pxCurrentTCB = NULL;

// 3. Task Creation (Simplified)
void CreateTask(TCB_t *tcb, const char *name, int priority) {
    strcpy(tcb->pcTaskName, name);
    tcb->uxPriority = priority;
    
    // Initialize Stack Pointer to the END of the array (Stack grows down!)
    tcb->pxTopOfStack = &(tcb->pxStack[STACK_SIZE - 1]);
    
    // In a real RTOS, we would "fake" a stack frame here 
    // (Push PC, LR, R0-R15) so the first context switch works.
    printf("[OS] Created Task: %s (Priority %d)\n", name, priority);
}

// 4. The Scheduler (Simplified)
void vTaskSwitchContext(TCB_t *taskA, TCB_t *taskB) {
    printf("\n[Scheduler] Switching from %s to %s\n", taskA->pcTaskName, taskB->pcTaskName);
    
    // Step 1: Save Context of A (Simulated)
    // In Assembly: PUSH {R4-R11}
    // Save the new SP to the TCB
    printf("  -> Saving Context of %s (SP: %p)\n", taskA->pcTaskName, taskA->pxTopOfStack);
    
    // Step 2: Update Current TCB
    pxCurrentTCB = taskB;
    
    // Step 3: Restore Context of B (Simulated)
    // Load SP from TCB
    // In Assembly: POP {R4-R11}
    printf("  -> Restoring Context of %s (SP: %p)\n", taskB->pcTaskName, taskB->pxTopOfStack);
}

int main() {
    TCB_t task1, task2;
    
    CreateTask(&task1, "SensorTask", 2);
    CreateTask(&task2, "DisplayTask", 1);
    
    // Start running Task 1
    pxCurrentTCB = &task1;
    printf("\n[OS] Running: %s\n", pxCurrentTCB->pcTaskName);
    
    // Simulate Tick Interrupt -> Switch to Task 2
    vTaskSwitchContext(&task1, &task2);
    
    // Simulate Tick Interrupt -> Switch back to Task 1
    vTaskSwitchContext(&task2, &task1);

    return 0;
}
