#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*
 * Phase 5: Heap Management Simulation
 * 
 * Demonstrates:
 * 1. Fragmentation (The "Swiss Cheese" problem).
 * 2. Why Coalescence (Merging free blocks) is critical.
 * 
 * Scenario:
 * - We have a 100 byte heap.
 * - We allocate 3 blocks of 20 bytes.
 * - We free the middle one.
 * - We try to allocate 40 bytes.
 */

#define HEAP_SIZE 100

typedef struct {
    uint8_t memory[HEAP_SIZE];
    bool is_allocated[HEAP_SIZE]; // Map of used bytes (Simplified)
} SimpleHeap_t;

SimpleHeap_t my_heap;

void heap_init() {
    memset(my_heap.is_allocated, 0, HEAP_SIZE);
}

// A dumb allocator (First Fit) - No Coalescence logic needed for this sim
// because we track byte-by-byte.
int heap_alloc(int size) {
    int consecutive_free = 0;
    int start_index = -1;

    for (int i = 0; i < HEAP_SIZE; i++) {
        if (!my_heap.is_allocated[i]) {
            if (consecutive_free == 0) start_index = i;
            consecutive_free++;
            if (consecutive_free == size) {
                // Found a block! Mark it used.
                for (int j = start_index; j < start_index + size; j++) {
                    my_heap.is_allocated[j] = true;
                }
                printf("[Heap] Allocated %d bytes at Index %d\n", size, start_index);
                return start_index;
            }
        } else {
            consecutive_free = 0;
            start_index = -1;
        }
    }
    printf("[Heap] FAILED to allocate %d bytes! (Fragmentation?)\n", size);
    return -1;
}

void heap_free(int start_index, int size) {
    for (int i = start_index; i < start_index + size; i++) {
        my_heap.is_allocated[i] = false;
    }
    printf("[Heap] Freed %d bytes at Index %d\n", size, start_index);
}

void print_heap_stats() {
    int free_bytes = 0;
    int max_block = 0;
    int current_block = 0;

    for (int i = 0; i < HEAP_SIZE; i++) {
        if (!my_heap.is_allocated[i]) {
            free_bytes++;
            current_block++;
        } else {
            if (current_block > max_block) max_block = current_block;
            current_block = 0;
        }
    }
    if (current_block > max_block) max_block = current_block;

    printf("Stats: Total Free: %d, Max Contiguous Block: %d\n", free_bytes, max_block);
}

int main() {
    heap_init();
    printf("=== Heap Fragmentation Simulation ===\n");
    print_heap_stats();

    // 1. Allocate 3 blocks
    int p1 = heap_alloc(20);
    int p2 = heap_alloc(20);
    int p3 = heap_alloc(20);
    print_heap_stats();

    // 2. Free the MIDDLE block
    // Now we have a hole.
    heap_free(p2, 20);
    print_heap_stats();

    // 3. Try to allocate a large block (40 bytes)
    // We have 60 bytes free total! (20 in middle + 40 at end)
    // But do we have 40 *contiguous*?
    printf("\nTrying to allocate 40 bytes...\n");
    heap_alloc(40); // Should succeed at the end (Index 60)

    // 4. Free the LAST block (p3)
    heap_free(p3, 20);
    print_heap_stats();
    
    // NOW: We have 20 bytes free at Index 20, and 20 bytes free at Index 40.
    // Total Free: 80 bytes.
    // But Max Contiguous: 20 bytes (if no coalescence) or 40 bytes (if coalesced)?
    // My simple simulator implicitly coalesces because it scans the array.
    // But imagine if 'p3' was still there.
    
    printf("\n--- Simulating Bad Fragmentation ---\n");
    // Reset
    heap_init();
    int a = heap_alloc(30);
    int b = heap_alloc(10); // Barrier
    int c = heap_alloc(30);
    int d = heap_alloc(10); // Barrier
    
    heap_free(a, 30); // Free first chunk
    heap_free(c, 30); // Free third chunk
    
    print_heap_stats();
    printf("Total Free is 60... but can I allocate 60?\n");
    heap_alloc(60); // Fail!

    return 0;
}
