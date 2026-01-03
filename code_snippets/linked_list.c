#include <stdio.h>
#include <stdlib.h>

/*
 * Doubly Linked List
 * The backbone of an RTOS Scheduler.
 * Used for: Ready List, Blocked List, Suspended List.
 */

// 1. The Node (Represents a Task in the list)
typedef struct Node {
    struct Node *next;
    struct Node *prev;
    int task_id;       // In a real RTOS, this would be the TCB
    int priority;
} Node_t;

// 2. The List (Represents a Queue, e.g., "Ready Queue")
typedef struct {
    Node_t *head;
    Node_t *tail;
    int count;
} List_t;

// Initialize the list
void list_init(List_t *list) {
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

// Add a task to the END of the list (Round Robin insertion)
void list_insert_end(List_t *list, Node_t *new_node) {
    if (list->head == NULL) {
        // List is empty
        list->head = new_node;
        list->tail = new_node;
        new_node->next = NULL;
        new_node->prev = NULL;
    } else {
        // Add to tail
        list->tail->next = new_node; // Old tail points to new
        new_node->prev = list->tail; // New points back to old tail
        new_node->next = NULL;       // New is now the end
        list->tail = new_node;       // Update tail pointer
    }
    list->count++;
}

// Remove a task from the list (e.g., when it blocks or is deleted)
void list_remove(List_t *list, Node_t *node) {
    if (list->head == NULL || node == NULL) return;

    // If node is head
    if (node == list->head) {
        list->head = node->next;
    }

    // If node is tail
    if (node == list->tail) {
        list->tail = node->prev;
    }

    // Unlink from neighbors
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    if (node->prev != NULL) {
        node->prev->next = node->next;
    }

    node->next = NULL;
    node->prev = NULL;
    list->count--;
}

void print_list(List_t *list) {
    Node_t *current = list->head;
    printf("List (Count %d): ", list->count);
    while (current != NULL) {
        printf("[Task %d] <-> ", current->task_id);
        current = current->next;
    }
    printf("NULL\n");
}

int main() {
    List_t ready_list;
    list_init(&ready_list);

    // Create some dummy tasks
    Node_t t1 = { .task_id = 1, .priority = 2 };
    Node_t t2 = { .task_id = 2, .priority = 2 };
    Node_t t3 = { .task_id = 3, .priority = 1 };

    printf("1. Adding Tasks...\n");
    list_insert_end(&ready_list, &t1);
    list_insert_end(&ready_list, &t2);
    list_insert_end(&ready_list, &t3);
    print_list(&ready_list);

    printf("2. Removing Task 2 (Middle)...\n");
    list_remove(&ready_list, &t2);
    print_list(&ready_list);

    printf("3. Removing Task 1 (Head)...\n");
    list_remove(&ready_list, &t1);
    print_list(&ready_list);

    return 0;
}
