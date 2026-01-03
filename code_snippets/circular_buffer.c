#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * Circular Buffer (Ring Buffer)
 * 
 * WHY: Used heavily in UART/Audio drivers.
 * - Producer (ISR) writes data.
 * - Consumer (Task) reads data.
 * - "Circular" means when we reach the end, we wrap around to index 0.
 * - No need to shift elements (O(1) complexity).
 */

#define BUFFER_SIZE 5

typedef struct {
    uint8_t buffer[BUFFER_SIZE];
    int head; // Write Index
    int tail; // Read Index
    int count;
} RingBuffer_t;

void rb_init(RingBuffer_t *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

bool rb_write(RingBuffer_t *rb, uint8_t data) {
    if (rb->count == BUFFER_SIZE) {
        printf("[Overflow] Buffer Full! Dropping %d\n", data);
        return false;
    }
    
    rb->buffer[rb->head] = data;
    // Wrap around logic: (0 -> 1 -> 2 -> 3 -> 4 -> 0)
    rb->head = (rb->head + 1) % BUFFER_SIZE;
    rb->count++;
    return true;
}

bool rb_read(RingBuffer_t *rb, uint8_t *data) {
    if (rb->count == 0) {
        printf("[Underflow] Buffer Empty!\n");
        return false;
    }
    
    *data = rb->buffer[rb->tail];
    // Wrap around logic
    rb->tail = (rb->tail + 1) % BUFFER_SIZE;
    rb->count--;
    return true;
}

void print_buffer(RingBuffer_t *rb) {
    printf("Buffer [Count %d]: ", rb->count);
    // Note: This print is just for debug, it doesn't respect the "Ring" visual perfectly
    for(int i=0; i<BUFFER_SIZE; i++) {
        if (i == rb->head) printf("H");
        if (i == rb->tail) printf("T");
        printf("[%d] ", rb->buffer[i]);
    }
    printf("\n");
}

int main() {
    RingBuffer_t rb;
    rb_init(&rb);
    uint8_t val;

    printf("1. Writing 3 bytes (10, 20, 30)...\n");
    rb_write(&rb, 10);
    rb_write(&rb, 20);
    rb_write(&rb, 30);
    print_buffer(&rb);

    printf("2. Reading 2 bytes...\n");
    rb_read(&rb, &val); printf("Read: %d\n", val);
    rb_read(&rb, &val); printf("Read: %d\n", val);
    print_buffer(&rb);

    printf("3. Writing 3 more (40, 50, 60) - WRAP AROUND!\n");
    rb_write(&rb, 40);
    rb_write(&rb, 50);
    rb_write(&rb, 60); // This should wrap to index 0
    print_buffer(&rb);

    printf("4. Writing one more (70) - OVERFLOW!\n");
    rb_write(&rb, 70);

    return 0;
}
