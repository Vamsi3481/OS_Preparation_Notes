# Phase 3: OS Fundamentals

## 1. Scheduling Algorithms
The Scheduler is the "Boss" that decides which task gets the CPU.

### A. Cooperative Scheduling (The "Polite" Way)
- **Concept**: Tasks run until they are done or explicitly say "I yield".
- **Pros**: Simple, no race conditions (usually).
- **Cons**: If one task hangs (`while(1)`), the whole system freezes.
- **Example**: Arduino `loop()`, simple infinite loops.

### B. Preemptive Scheduling (The "Bossy" Way)
- **Concept**: The OS can forcibly pause a task to run a more important one.
- **Mechanism**: Relies on a Hardware Timer Interrupt (The Tick).
- **Pros**: Deterministic. High priority tasks (like Motor Control) run immediately.
- **Cons**: Complex. Race conditions possible.

### C. Algorithms
1.  **First Come First Serve (FCFS)**:
    - Simple Queue.
    - **Problem**: "Convoy Effect". If a big task is first, everyone waits.
2.  **Round Robin (Time Slicing)**:
    - Each task gets a fixed slice (e.g., 10ms).
    - **Fairness**: Everyone gets a turn.
    - **Used in**: Desktop OS (Windows/Linux).
3.  **Priority Scheduling (RTOS Standard)**:
    - Highest Priority ALWAYS runs.
    - If a High Priority task wakes up, it preempts the Low Priority task immediately.
    - **Used in**: FreeRTOS, Zephyr, QNX.

## 2. The "Tick"
The heartbeat of the OS. A hardware timer that fires periodically (e.g., every 1ms).
- It wakes up the Scheduler.
- It counts time for `vTaskDelay`.

## 3. Interrupts & ISRs
An **Interrupt** is a hardware signal that forces the CPU to stop what it's doing and jump to a specific function (**ISR** - Interrupt Service Routine).

### Hardware Stacking (ARM Cortex-M)
When an interrupt fires, the hardware automatically pushes 8 registers to the stack:
- **R0-R3**: Caller-saved registers (Arguments).
- **R12**: Intra-procedure-call scratch register.
- **LR**: Link Register.
- **PC**: Program Counter (Return Address).
- **xPSR**: Program Status Register (Flags).

**Why not R4-R11?**
These are "Callee-saved". The ISR is responsible for saving them *if* it uses them. This optimization makes interrupts faster.

### The Golden Rule of ISRs
**"Keep it Short!"**
- An ISR blocks other interrupts (or lower priority tasks).
- **NEVER** use `printf` (it blocks).
- **NEVER** use `malloc` (it's slow/not thread-safe).
- **NEVER** wait for anything (`while` loops).

### Deferred Interrupt Processing (Top Half / Bottom Half)
Since ISRs must be short, we split the work:
1.  **Top Half (The ISR)**:
    - Acknowledge hardware (Clear flag).
    - Read critical data (e.g., from register).
    - Signal a Task (Semaphore/Queue).
    - **Exit immediately.**
2.  **Bottom Half (The Task)**:
    - Wake up when signaled.
    - Do the heavy processing (Math, Print, Save to Flash).

## 4. Concurrency & Synchronization
### The Race Condition
When two tasks try to Modify the same shared variable at the same time.
- **Cause**: The "Read-Modify-Write" cycle is not atomic. A context switch can happen in the middle.
- **Fix**: Critical Sections (Locks).

### Mutex vs Semaphore (The Interview Question)
| Feature | Mutex | Binary Semaphore |
| :--- | :--- | :--- |
| **Purpose** | **Locking** (Mutual Exclusion) | **Signaling** (Synchronization) |
| **Ownership** | **YES**. Only the task that locked it can unlock it. | **NO**. ISR can "give" (unlock), Task can "take" (lock). |
| **Priority Inheritance** | **YES**. Solves Priority Inversion. | **NO**. |

### Advanced Problems
1.  **Priority Inversion**: High Prio task waits for Low Prio task (who holds mutex). Medium Prio task preempts Low, making High wait forever.
    - **Fix**: **Priority Inheritance**. (Low Prio task temporarily gets High Prio).
2.  **Deadlock**: Task A has Lock 1, wants Lock 2. Task B has Lock 2, wants Lock 1.
    - **Fix**: Always acquire locks in the **same order**.
