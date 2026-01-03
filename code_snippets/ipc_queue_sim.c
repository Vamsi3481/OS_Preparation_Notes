#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*
 * IPC: FreeRTOS Queues (Copy by Value)
 * 
 * This demo proves that FreeRTOS copies the DATA, not just the pointer.
 * 
 * Scenario:
 * 1. Task A creates a local variable 'msg'.
 * 2. Task A sends 'msg' to the Queue.
 * 3. Task A changes 'msg'.
 * 4. Task B reads from Queue.
 * 
 * Question: Does Task B see the OLD value or the NEW value?
 * Answer: The OLD value (because it was copied).
 */

typedef struct {
    int id;
    char text[10];
} Message_t;

// Simulated Queue Buffer
Message_t queue_buffer[5];
int queue_head = 0;
int queue_tail = 0;
int queue_count = 0;

void send_to_queue(Message_t *msg) {
    // COPY BY VALUE: We copy the content of the struct into the buffer
    queue_buffer[queue_head] = *msg; 
    
    queue_head = (queue_head + 1) % 5;
    queue_count++;
    printf("[Queue] Data copied into queue. (ID: %d, Text: %s)\n", msg->id, msg->text);
}

void receive_from_queue(Message_t *buffer) {
    if (queue_count > 0) {
        // COPY BY VALUE: We copy from buffer to the receiver's variable
        *buffer = queue_buffer[queue_tail];
        
        queue_tail = (queue_tail + 1) % 5;
        queue_count--;
    }
}

int main() {
    printf("=== FreeRTOS Queue Simulation (Copy by Value) ===\n");

    // --- Task A (Sender) ---
    {
        Message_t my_msg;
        my_msg.id = 1;
        strcpy(my_msg.text, "Hello");
        
        printf("Task A: Sending message (ID: 1, Text: Hello)...\n");
        send_to_queue(&my_msg);
        
        // NOW, Task A modifies its local variable
        printf("Task A: Modifying local variable to 'Goodbye'...\n");
        my_msg.id = 99;
        strcpy(my_msg.text, "Goodbye");
    }

    // --- Task B (Receiver) ---
    {
        Message_t received_msg;
        printf("\nTask B: Receiving from queue...\n");
        receive_from_queue(&received_msg);
        
        printf("Task B: Received ID: %d, Text: %s\n", received_msg.id, received_msg.text);
        
        if (received_msg.id == 1) {
            printf("RESULT: SUCCESS! Queue preserved the original data.\n");
        } else {
            printf("RESULT: FAIL! Queue saw the modified data (Reference).\n");
        }
    }

    return 0;
}
