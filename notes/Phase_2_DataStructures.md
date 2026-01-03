# Phase 2: Core Data Structures & Embedded C

## 1. Pointers Deep Dive
- **void***: Generic pointer. Must cast before use. Used in FreeRTOS Queues.
- **Function Pointers**: Points to code. Used for Callbacks/ISRs.
- **Double Pointers (`int**`)**: Points to a pointer. Used to modify *where* a pointer points (e.g., `malloc` wrappers).

### The "Pass by Value" Rule
In C, function arguments are **copies**.
- If you pass `p` (value `NULL`), the function gets a copy of `NULL`.
- To change `p`, you must pass `&p` (address of `p`).

## 2. Structs & Padding
- **Padding**: The compiler inserts empty bytes to align data to CPU word boundaries (4 bytes on 32-bit CPU).
- **Packed**: `__attribute__((packed))` forces no padding. Essential for:
    - Network Packets (WiFi/BLE headers).
    - Hardware Register Maps.

## 3. Unions
- **Shared Memory**: All members share the *same* address.
- **Use Case**: Viewing the same data in different ways (e.g., `uint32_t` vs `uint8_t[4]`).

## 4. Bitfields
- **Bit-level access**: `uint8_t enable : 1;` uses only 1 bit.
- **Use Case**: Control Registers (Flags).
