#include <stdio.h>
#include <stdint.h>

/*
 * Phase 0: The Interrupt Vector Table
 * 
 * WHAT: An array of function pointers stored at the beginning of Flash (0x00000000).
 * WHY: When an interrupt fires (e.g., IRQ #1), the CPU looks at index 1 of this table
 *      to find the address of the ISR to execute.
 */

// Define a function pointer type for ISRs
typedef void (*ISR_Handler_t)(void);

// Simulated ISRs
void Reset_Handler(void) {
    printf("[CPU] Executing Reset_Handler (Startup Code)...\n");
}

void NMI_Handler(void) {
    printf("[CPU] Executing NMI_Handler (Non-Maskable Interrupt)!\n");
}

void HardFault_Handler(void) {
    printf("[CPU] CRITICAL: HardFault_Handler! System Crashed.\n");
    while(1); // Trap
}

void SysTick_Handler(void) {
    printf("[CPU] SysTick_Handler (OS Tick).\n");
}

// The Vector Table (Simulated)
// In real hardware, this is placed at 0x00000000 via Linker Script (.isr_vector)
ISR_Handler_t vector_table[] = {
    (ISR_Handler_t)0x20001000, // 0: Initial Stack Pointer (MSP) - Not a function!
    Reset_Handler,             // 1: Reset
    NMI_Handler,               // 2: NMI
    HardFault_Handler,         // 3: HardFault
    0, 0, 0, 0,                // 4-7: Reserved
    0, 0, 0,                   // 8-10: SVCall, Debug, etc.
    SysTick_Handler            // 15: SysTick
};

// Simulate the Hardware Interrupt Controller (NVIC)
void NVIC_Simulate_Interrupt(int irq_number) {
    printf("\n[NVIC] Interrupt #%d fired!\n", irq_number);
    
    if (irq_number < 0 || irq_number >= sizeof(vector_table)/sizeof(vector_table[0])) {
        printf("[CPU] Error: Invalid IRQ number.\n");
        return;
    }

    // 1. Fetch the address from the Vector Table
    ISR_Handler_t handler = vector_table[irq_number];
    
    // 2. Check if handler exists
    if (handler == 0) {
        printf("[CPU] Error: No handler defined for IRQ #%d (Default Handler?)\n", irq_number);
        // In real HW, this usually falls through to a Default_Handler (infinite loop)
        return;
    }
    
    // 3. Jump to the Handler
    // (In real HW, this involves stacking R0-R3, R12, LR, PC, xPSR first)
    handler();
}

int main() {
    printf("=== Interrupt Vector Table Simulation ===\n");
    printf("Vector Table Address: %p\n", (void*)vector_table);
    
    // 1. Simulate Power On Reset
    NVIC_Simulate_Interrupt(1); // Reset_Handler
    
    // 2. Simulate OS Tick
    NVIC_Simulate_Interrupt(15); // SysTick
    
    // 3. Simulate a Crash
    NVIC_Simulate_Interrupt(3); // HardFault

    return 0;
}
