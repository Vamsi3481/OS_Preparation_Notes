# Phase 0: Hardware & Architecture Basics

## 1. The CPU "Scratchpad" (Registers)
The CPU cannot operate on RAM directly. It must load data into **Registers**.
- **R0-R12**: General Purpose (Math, variables).
- **R13 (SP - Stack Pointer)**: Points to the top of the Stack.
- **R14 (LR - Link Register)**: Holds the return address when a function is called.
- **R15 (PC - Program Counter)**: Holds the address of the *next* instruction to execute.

## 2. The Fetch-Decode-Execute Cycle (The Heartbeat)
The CPU is a state machine that loops forever:
1.  **FETCH**: Read instruction from Flash (TEXT) at address `PC`. Increment `PC`.
2.  **DECODE**: Figure out what the instruction means (e.g., `ADD R0, R1`).
3.  **EXECUTE**: Do the work (ALU adds the numbers).
    - *Note*: In modern CPUs (Cortex-M), this is **Pipelined**. (Fetching Instr 3, Decoding Instr 2, Executing Instr 1 simultaneously).

## 3. Memory Hierarchy (The Speed Ladder)
Why do we have Cache? Because Flash is slow.
1.  **Registers**: Instant (0 cycles).
2.  **L1 Cache**: Very Fast (1-2 cycles). Small (e.g., 32KB).
3.  **Internal SRAM**: Fast (e.g., 100MHz).
4.  **Flash (Code)**: Slow (e.g., 20MHz). The CPU often has to "wait" (Wait States) to read instructions.
5.  **External RAM (DDR)**: Very Slow.

**Implication for RTOS**:
- **Cache Coherency**: If DMA writes to RAM, the Cache might still hold old data. You must **Invalidate** Cache before reading DMA data.

## 4. The Interrupt Vector Table
- **What**: An array of Function Pointers.
- **Where**: Always at the start of memory (Address `0x00000000` or `0x08000000`).
- **Content**:
    - Index 0: Initial Stack Pointer (MSP).
    - Index 1: Reset Handler (The first code that runs).
    - Index 15: SysTick (OS Timer).
- **Mechanism**: When IRQ #N fires, Hardware fetches address at `Table[N]` and jumps there.

## 5. Memory Layout
A C program sees memory in 4 main segments:
1.  **TEXT**: Code instructions. Read-Only (Flash).
2.  **DATA/BSS**: Global variables.
    - *Data*: Initialized (`int x = 10;`)
    - *BSS*: Uninitialized (`int y;` -> Zeroed out).
3.  **HEAP**: Dynamic memory (`malloc`). Grows **UP**.
4.  **STACK**: Local variables & function calls. Grows **DOWN**.
