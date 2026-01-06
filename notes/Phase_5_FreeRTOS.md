# Phase 5: FreeRTOS Deep Dive

## 1. Memory Management (The 5 Heaps)
FreeRTOS provides 5 implementations of `pvPortMalloc` and `vPortFree`. You must choose one in `FreeRTOSConfig.h`.

| Heap Scheme | Characteristics | Use Case |
| :--- | :--- | :--- |
| **heap_1.c** | **Alloc Only**. No `free()`. Deterministic. | Safety-critical systems where tasks are never deleted (e.g., Airbag). |
| **heap_2.c** | Best Fit algorithm. Allows `free()`. **Fragmentation issues**. | **Legacy**. Do not use. Replaced by heap_4. |
| **heap_3.c** | Wraps standard C `malloc()`/`free()`. | When you want to use the Linker's heap (requires thread-safe `malloc`). |
| **heap_4.c** | **Coalescence**. Combines adjacent free blocks. | **General Purpose**. The default for most projects. |
| **heap_5.c** | Same as heap_4, but supports **Discontiguous Memory** (e.g., Fast RAM + Slow RAM). | Systems with multiple RAM banks. |

### Fragmentation
- **Problem**: You have 10KB free total, but in tiny 100B chunks. You try to allocate 1KB and fail.
- **Solution**: `heap_4` "Coalesces" (merges) free blocks when they are freed.

## 2. Queue Internals (The "Blocked Lists")
A Queue is not just a buffer. It contains **Two Linked Lists**:
1.  **`xTasksWaitingToSend`**: Tasks blocked because the queue is FULL.
2.  **`xTasksWaitingToReceive`**: Tasks blocked because the queue is EMPTY.

**Mechanism**:
- When Task A tries to write to a FULL queue, it is removed from the **Ready List** and added to `xTasksWaitingToSend`.
- When Task B reads from the queue (making space), the OS checks `xTasksWaitingToSend`.
- It picks the highest priority task, removes it from the Waiting List, and moves it back to the **Ready List**.

## 3. Task Creation API
To create a task in FreeRTOS, you use `xTaskCreate`.

```c
BaseType_t xTaskCreate(
    TaskFunction_t pxTaskCode,   // Function pointer (The "main" of the task)
    const char * const pcName,   // Debug name ("SensorTask")
    const uint16_t usStackDepth, // Stack size in WORDS (not bytes!)
    void * const pvParameters,   // Argument passed to the task
    UBaseType_t uxPriority,      // Priority (0 = Low, configMAX_PRIORITIES-1 = High)
    TaskHandle_t * const pxCreatedTask // Handle (pointer) to the TCB (Optional)
);
```

## 4. FreeRTOSConfig.h
The "Control Panel" of the OS.
- `configUSE_PREEMPTION`: 1 for Preemptive, 0 for Cooperative.
- `configTICK_RATE_HZ`: How fast the Tick fires (usually 1000Hz = 1ms).
- `configTOTAL_HEAP_SIZE`: How much RAM the OS reserves for the Heap.
- `configMAX_PRIORITIES`: Number of priority levels (saves RAM if lower).
- `configMINIMAL_STACK_SIZE`: The stack size for the Idle Task.

## 5. ISR Safe APIs
In FreeRTOS, you **cannot** call standard API functions from an ISR.
You must use the `FromISR` versions.
- **Task**: `xQueueSend(q, &data, timeout)`
- **ISR**: `xQueueSendFromISR(q, &data, &xHigherPriorityTaskWoken)`

**Why?**
1.  ISRs cannot block (wait). The `timeout` parameter must be ignored.
2.  ISRs might wake up a high-priority task. We need to know if we should trigger a context switch *immediately* after the ISR exits.

## 6. Software Timers
Hardware timers are scarce (maybe 3-4 per chip). Software timers allow unlimited timers.
- **Mechanism**: Managed by a system task called the **Daemon Task** (or Timer Service Task).
- **Types**:
    1.  **One-Shot**: Runs once (e.g., "Turn off backlight").
    2.  **Auto-Reload**: Runs periodically (e.g., "Blink LED").
- **Constraint**: The Callback runs in the context of the Daemon Task. **NEVER BLOCK** in a timer callback (no `vTaskDelay`, no `printf`), or you freeze all other timers!
