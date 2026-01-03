#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
 * OS Scheduling Simulation
 * Demonstrates:
 * 1. Cooperative Scheduling (Run until completion/yield)
 * 2. Preemptive Round Robin (Time slicing)
 * 3. Priority Scheduling (Highest priority runs first)
 */

typedef struct {
    int id;
    int burst_time;    // How long the task needs to run
    int remaining_time;
    int priority;      // Higher number = Higher priority
} Task_t;

void run_fcfs(Task_t tasks[], int count) {
    printf("\n--- FCFS (First Come First Serve) ---\n");
    int time = 0;
    for (int i = 0; i < count; i++) {
        printf("Time %d: Task %d starts (Duration %d)\n", time, tasks[i].id, tasks[i].burst_time);
        time += tasks[i].burst_time;
        printf("Time %d: Task %d finishes\n", time, tasks[i].id);
    }
}

void run_round_robin(Task_t tasks[], int count, int time_slice) {
    printf("\n--- Round Robin (Time Slice: %d) ---\n");
    int time = 0;
    int tasks_left = count;
    
    // Reset remaining time
    for(int i=0; i<count; i++) tasks[i].remaining_time = tasks[i].burst_time;

    while (tasks_left > 0) {
        bool idle = true;
        for (int i = 0; i < count; i++) {
            if (tasks[i].remaining_time > 0) {
                idle = false;
                int run_time = (tasks[i].remaining_time > time_slice) ? time_slice : tasks[i].remaining_time;
                
                printf("Time %d: Task %d runs for %d ms\n", time, tasks[i].id, run_time);
                time += run_time;
                tasks[i].remaining_time -= run_time;

                if (tasks[i].remaining_time == 0) {
                    printf("Time %d: Task %d COMPLETED\n", time, tasks[i].id);
                    tasks_left--;
                }
            }
        }
        if (idle) break;
    }
}

void run_priority(Task_t tasks[], int count) {
    printf("\n--- Priority Scheduling (Preemptive Simulation) ---\n");
    // Simple bubble sort to order by Priority (Highest First)
    // In a real OS, this is done by the Ready List (O(1) insertion)
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (tasks[j].priority < tasks[j+1].priority) {
                Task_t temp = tasks[j];
                tasks[j] = tasks[j+1];
                tasks[j+1] = temp;
            }
        }
    }

    int time = 0;
    for (int i = 0; i < count; i++) {
        printf("Time %d: Task %d (Prio %d) starts\n", time, tasks[i].id, tasks[i].priority);
        time += tasks[i].burst_time;
        printf("Time %d: Task %d finishes\n", time, tasks[i].id);
    }
}

int main() {
    Task_t task_list[] = {
        {1, 10, 10, 1}, // Low Prio
        {2, 5,  5,  3}, // High Prio
        {3, 8,  8,  2}  // Medium Prio
    };
    int count = 3;

    run_fcfs(task_list, count);
    
    // Note: Array passed by reference, but we reset remaining_time inside
    run_round_robin(task_list, count, 4); // 4ms Time Slice

    // Note: This sorts the array, destroying original order
    run_priority(task_list, count);

    return 0;
}
