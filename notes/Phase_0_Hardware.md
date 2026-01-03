# Phase 0: Hardware & Architecture Basics

## 1. The CPU "Scratchpad" (Registers)
The CPU cannot operate on RAM directly. It must load data into **Registers**.
- **R0-R12**: General Purpose (Math, variables).
- **R13 (SP - Stack Pointer)**: Points to the top of the Stack.
- **R14 (LR - Link Register)**: Holds the return address when a function is called.
- **R15 (PC - Program Counter)**: Holds the address of the *next* instruction to execute.

## 2. Memory Layout
A C program sees memory in 4 main segments:
1.  **TEXT**: Code instructions. Read-Only (Flash).
2.  **DATA/BSS**: Global variables.
    - *Data*: Initialized (`int x = 10;`)
    - *BSS*: Uninitialized (`int y;` -> Zeroed out).
3.  **HEAP**: Dynamic memory (`malloc`). Grows **UP**.
4.  **STACK**: Local variables & function calls. Grows **DOWN**.

## 3. The Stack & Stack Frames
- **Growth**: The Stack grows from High Address -> Low Address.
- **Stack Frame**: Pushed for every function call. Contains:
    - Local Variables.
    - Return Address (LR).
    - Saved Registers (to restore caller's state).
- **Stack Overflow**: When Stack grows into Heap/Data. Causes corruption or HardFault.

## 4. Key Takeaway for RTOS
- **Context Switching** relies on saving these Registers (R0-R15) to the Stack and restoring them later.
