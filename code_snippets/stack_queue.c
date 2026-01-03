#include <stdio.h>
#include <stdbool.h>

#define MAX_SIZE 5

/*
 * 1. Stack (LIFO - Last In First Out)
 * Used for: Function calls, Local variables, Expression evaluation.
 */
typedef struct {
    int items[MAX_SIZE];
    int top;
} Stack_t;

void stack_init(Stack_t *s) { s->top = -1; }

bool stack_push(Stack_t *s, int val) {
    if (s->top == MAX_SIZE - 1) return false;
    s->items[++(s->top)] = val;
    return true;
}

bool stack_pop(Stack_t *s, int *val) {
    if (s->top == -1) return false;
    *val = s->items[(s->top)--];
    return true;
}

/*
 * 2. Queue (FIFO - First In First Out)
 * Used for: Task Scheduling, Message Passing.
 * Note: This is a "Linear Queue". It has a problem:
 * Once 'rear' hits the end, we can't insert even if 'front' is empty.
 * That's why Ring Buffers are better!
 */
typedef struct {
    int items[MAX_SIZE];
    int front;
    int rear;
} Queue_t;

void queue_init(Queue_t *q) { q->front = -1; q->rear = -1; }

bool queue_enqueue(Queue_t *q, int val) {
    if (q->rear == MAX_SIZE - 1) return false;
    if (q->front == -1) q->front = 0;
    q->items[++(q->rear)] = val;
    return true;
}

bool queue_dequeue(Queue_t *q, int *val) {
    if (q->front == -1 || q->front > q->rear) return false;
    *val = q->items[(q->front)++];
    return true;
}

int main() {
    printf("=== Stack (LIFO) ===\n");
    Stack_t s; stack_init(&s);
    stack_push(&s, 1); stack_push(&s, 2); stack_push(&s, 3);
    int val;
    while(stack_pop(&s, &val)) printf("Popped: %d\n", val);

    printf("\n=== Queue (FIFO) ===\n");
    Queue_t q; queue_init(&q);
    queue_enqueue(&q, 10); queue_enqueue(&q, 20); queue_enqueue(&q, 30);
    while(queue_dequeue(&q, &val)) printf("Dequeued: %d\n", val);

    return 0;
}
