#include <stdio.h>
#include <stdint.h>

/*
 * 1. Structs & Memory Alignment
 * Structs group data. But compilers add "padding" to align data.
 * This is CRITICAL when mapping structs to hardware registers or network packets.
 */
struct NormalStruct {
    uint8_t  a; // 1 byte
    uint32_t b; // 4 bytes
    uint8_t  c; // 1 byte
    // Expected size: 1 + 4 + 1 = 6 bytes?
    // Actual size: Usually 12 bytes due to padding!
};

// Packed struct: No padding. Used for HW Registers / Network Packets.
struct __attribute__((packed)) PackedStruct {
    uint8_t  a;
    uint32_t b;
    uint8_t  c;
};

/*
 * 2. Unions
 * Unions allow multiple members to share the SAME memory location.
 * Size of union = Size of largest member.
 * Used for: Converting types (e.g., bytes <-> float) or Register Access.
 */
union DataConverter {
    uint32_t as_int;
    uint8_t  as_bytes[4];
};

/*
 * 3. Bitfields
 * Used to access specific bits within a byte/word.
 * Extremely common in Register Maps (e.g., Control Registers).
 */
struct ControlRegister {
    uint8_t enable      : 1; // Bit 0
    uint8_t mode        : 2; // Bits 1-2
    uint8_t interrupt   : 1; // Bit 3
    uint8_t reserved    : 4; // Bits 4-7
};

int main() {
    printf("=== 1. Struct Padding ===\n");
    printf("Size of NormalStruct: %zu bytes\n", sizeof(struct NormalStruct));
    printf("Size of PackedStruct: %zu bytes\n", sizeof(struct PackedStruct));

    printf("\n=== 2. Union (Data Conversion) ===\n");
    union DataConverter data;
    data.as_int = 0x12345678; // Little Endian: 78 56 34 12
    printf("Int: 0x%X\n", data.as_int);
    printf("Bytes: [0x%X, 0x%X, 0x%X, 0x%X]\n", 
           data.as_bytes[0], data.as_bytes[1], data.as_bytes[2], data.as_bytes[3]);

    printf("\n=== 3. Bitfields ===\n");
    struct ControlRegister reg = {0};
    reg.enable = 1;
    reg.mode = 3; // Binary 11
    reg.interrupt = 1;
    
    // Cast the whole struct to a byte to see the raw value
    uint8_t *raw_val = (uint8_t*)&reg;
    printf("Register Value: 0x%X\n", *raw_val);
    
    return 0;
}
