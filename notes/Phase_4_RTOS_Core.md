# Phase 4: RTOS Core Concepts

## 1. The Task Control Block (TCB)
The TCB is the "Soul" of the task. It contains:
- **`pxTopOfStack`**: Pointer to the top of the task's stack. **Always the first member** for Assembly optimization (`LDR R0, [R1]`).
- **`pcTaskName`**: Debug name.
- **`uxPriority`**: Priority level.
- **`xStateListItem`**: Links the task to Ready/Blocked lists.

## 2. The Context Switch (Deep Dive)
When the SysTick timer fires, it triggers the `PendSV` (Pendable Service Call) exception. This is where the magic happens.

### The Assembly View (ARM Cortex-M)
```assembly
PendSV_Handler:
    MRS R0, PSP          ; Read Process Stack Pointer (Current Task Stack)
    STMDB R0!, {R4-R11}  ; Save "Callee-Saved" registers to the Task Stack
    
    LDR R1, =pxCurrentTCB ; Get address of Current TCB
    LDR R1, [R1]          ; Dereference to get the TCB itself
    STR R0, [R1]          ; Save the new SP into TCB->pxTopOfStack (First member!)
    
    BL vTaskSwitchContext ; Call C function to pick the next task
    
    LDR R1, =pxCurrentTCB ; Get address of New TCB
    LDR R1, [R1]          ; Dereference
    LDR R0, [R1]          ; Load SP from New TCB->pxTopOfStack
    
    LDMIA R0!, {R4-R11}  ; Restore "Callee-Saved" registers from New Stack
    MSR PSP, R0          ; Write new SP to CPU register
    BX LR                ; Return from Interrupt (Hardware restores R0-R3, PC, etc.)
```

## 3. Stack Overflow Detection
When a task uses more stack than allocated, it corrupts memory (often the TCB of another task).
FreeRTOS offers 2 methods:
- **Method 1**: Check SP limit during Context Switch.
    - Fast.
    - **Flaw**: If overflow happens *and* recovers between switches, it misses it.
- **Method 2 (The Canary)**:
    - Fill stack with `0xA5A5A5A5` at creation.
    - Check the *end* of the stack during Context Switch.
    - If the value is not `0xA5`, overflow occurred.

## 4. Inter-Process Communication (IPC)
Tasks need to talk to each other. Global variables are dangerous (Race Conditions).
We use **Queues**.

### FreeRTOS Queues: "Copy by Value"
- When you send data to a queue, the OS makes a **byte-for-byte copy** of the data into the queue's internal buffer.
- **Pros**:
    - Safety: The sender can modify/destroy the original variable immediately after sending.
    - Decoupling: Sender and Receiver don't need to share memory scope.
- **Cons**:
    - Slow for large data (structs).
- **Optimization**: For large data, send a **Pointer** to the data (Queue of pointers).

### Advanced IPC
1.  **Event Groups**:
    - **Concept**: A 32-bit integer where each bit is a flag (e.g., Bit 0 = WiFi, Bit 1 = BLE).
    - **Logic**: Wait for (Bit 0 OR Bit 1) or (Bit 0 AND Bit 1).
    - **Use Case**: "Wait for Network AND FileSystem before starting upload."
2.  **Task Notifications**:
    - **Concept**: A 32-bit value *inside* the TCB.
    - **Speed**: **45% Faster** than Semaphores/Queues (No new object to allocate).
    - **Use Case**: Simple signaling (Wake up task from ISR).
    - **Limitation**: Can only send to ONE specific task. (Queues can have multiple readers).
3.  **Mailboxes**:
    - In FreeRTOS, a Mailbox is just a **Queue of Length 1** that overwrites the old value.
