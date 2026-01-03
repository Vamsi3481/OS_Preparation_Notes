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

## 2. FreeRTOSConfig.h
The "Control Panel" of the OS.
- `configUSE_PREEMPTION`: 1 for Preemptive, 0 for Cooperative.
- `configTICK_RATE_HZ`: How fast the Tick fires (usually 1000Hz = 1ms).
- `configTOTAL_HEAP_SIZE`: How much RAM the OS reserves for the Heap.
- `configMAX_PRIORITIES`: Number of priority levels (saves RAM if lower).
- `configMINIMAL_STACK_SIZE`: The stack size for the Idle Task.

## 3. ISR Safe APIs
In FreeRTOS, you **cannot** call standard API functions from an ISR.
You must use the `FromISR` versions.
- **Task**: `xQueueSend(q, &data, timeout)`
- **ISR**: `xQueueSendFromISR(q, &data, &xHigherPriorityTaskWoken)`

**Why?**
1.  ISRs cannot block (wait). The `timeout` parameter must be ignored.
2.  ISRs might wake up a high-priority task. We need to know if we should trigger a context switch *immediately* after the ISR exits.
