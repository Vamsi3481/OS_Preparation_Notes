#include <stdio.h>
#include <stdbool.h>

/*
 * Phase 4: Mailboxes
 * 
 * Concept:
 * - A Mailbox is a Queue of Length 1.
 * - Key Feature: "Overwrite". If full, it deletes the old value and writes the new one.
 * - Use Case: "Latest Sensor Reading" (we don't care about history, only the newest value).
 */

typedef struct {
    int value;
    bool is_full;
} Mailbox_t;

Mailbox_t my_mailbox;

void mailbox_write(int val) {
    // "Overwrite" behavior
    my_mailbox.value = val;
    my_mailbox.is_full = true;
    printf("[Mailbox] Wrote %d (Overwriting old value if any)\n", val);
}

int mailbox_read(int *val) {
    if (my_mailbox.is_full) {
        *val = my_mailbox.value;
        // In FreeRTOS, xQueuePeek reads without removing. xQueueReceive removes.
        // Here we simulate xQueuePeek (Broadcast style) or xQueueReceive depending on need.
        return 1; // Success
    }
    return 0; // Empty
}

int main() {
    printf("=== Mailbox Simulation ===\n");
    
    // 1. Write initial value
    mailbox_write(100);
    
    // 2. Overwrite it (Sensor updated)
    mailbox_write(200); 
    
    // 3. Read
    int val;
    if (mailbox_read(&val)) {
        printf("Read: %d (Expected 200)\n", val);
    }
    
    return 0;
}
