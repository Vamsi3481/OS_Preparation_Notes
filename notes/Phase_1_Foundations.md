# Phase 1: Absolute Foundations

## 1. Program vs Process vs Thread

| Term | Definition | Context |
| :--- | :--- | :--- |
| **Program** | Passive file on disk (e.g., `main.bin`). | Storage |
| **Process** | A program in execution. Has its own RAM (Stack, Heap, Data). Isolated. | Desktop OS (Linux/Windows) |
| **Thread** | A unit of execution *within* a process. | **RTOS / Embedded** |

## 2. The RTOS Model
In an RTOS (FreeRTOS, Zephyr), the entire firmware is **One Process**.
- We break the work into multiple **Threads** (called **Tasks**).

### Shared vs Private Resources
| Resource | Shared? | Notes |
| :--- | :--- | :--- |
| **Code (Text)** | YES | All tasks run the same code. |
| **Global Vars** | YES | **Danger!** Race conditions happen here. |
| **Heap** | YES | `malloc` is shared. Thread-safe `malloc` needed. |
| **Stack** | **NO** | Each Task has its own private Stack. |
| **Registers** | **NO** | Each Task has its own "saved" register state (Context). |

## 3. Why separate Stacks?
Since each task executes independently, they need their own history of function calls. Task A might be deep in `sensor_read()`, while Task B is waiting in `main()`. They cannot share a call stack.

## 4. Context Switching
The process of saving the state of the current task and restoring the state of the next task.

- **What is saved?** The CPU Registers (R0-R15).
- **Where is it saved?** On the **Task's own Stack**.
- **What is NOT saved?** The Heap, Global Variables (because they are shared and stay in memory).

### The Sequence
1.  **Interrupt**: Tick timer fires.
2.  **Save**: Push R0-R15 to Task A's Stack.
3.  **Switch SP**: Change CPU's Stack Pointer from Task A's Stack to Task B's Stack.
4.  **Restore**: Pop R0-R15 from Task B's Stack.
5.  **Resume**: Execute from where Task B left off.
