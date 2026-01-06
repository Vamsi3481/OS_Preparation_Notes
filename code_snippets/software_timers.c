#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/*
 * Phase 5: Software Timers
 * 
 * Concept:
 * - Hardware Timers are scarce (maybe 3-4 per chip).
 * - Software Timers allow you to have unlimited timers.
 * - They are managed by a single system task: The "Daemon Task" (or Timer Service Task).
 * 
 * Types:
 * 1. One-Shot: Runs once, then stops. (e.g., "Turn off LED after 5s").
 * 2. Auto-Reload: Runs periodically. (e.g., "Blink LED every 1s").
 */

typedef enum {
    TIMER_ONE_SHOT,
    TIMER_AUTO_RELOAD
} TimerType_t;

typedef struct {
    const char *name;
    TimerType_t type;
    int period_ticks;
    int remaining_ticks;
    bool is_active;
    void (*callback)(const char*);
} SoftwareTimer_t;

// The Timer Service Task (Daemon) manages this list
SoftwareTimer_t timers[2];

void my_timer_callback(const char *name) {
    printf("[Timer Callback] %s fired!\n", name);
}

void create_timers() {
    // Timer 1: One-Shot (Backlight off after 3 ticks)
    timers[0].name = "BacklightTimer";
    timers[0].type = TIMER_ONE_SHOT;
    timers[0].period_ticks = 3;
    timers[0].remaining_ticks = 3;
    timers[0].is_active = true;
    timers[0].callback = my_timer_callback;

    // Timer 2: Auto-Reload (Heartbeat every 2 ticks)
    timers[1].name = "HeartbeatTimer";
    timers[1].type = TIMER_AUTO_RELOAD;
    timers[1].period_ticks = 2;
    timers[1].remaining_ticks = 2;
    timers[1].is_active = true;
    timers[1].callback = my_timer_callback;
}

// Simulate the Daemon Task processing the timers
void process_timers_tick() {
    for (int i = 0; i < 2; i++) {
        if (timers[i].is_active) {
            timers[i].remaining_ticks--;
            
            if (timers[i].remaining_ticks == 0) {
                // Timer Expired! Call the callback.
                timers[i].callback(timers[i].name);
                
                // Logic for Type
                if (timers[i].type == TIMER_AUTO_RELOAD) {
                    timers[i].remaining_ticks = timers[i].period_ticks; // Reload
                } else {
                    timers[i].is_active = false; // Stop
                    printf("[Timer System] %s stopped.\n", timers[i].name);
                }
            }
        }
    }
}

int main() {
    printf("=== FreeRTOS Software Timer Simulation ===\n");
    create_timers();

    // Simulate 10 Ticks of the OS
    for (int tick = 1; tick <= 10; tick++) {
        printf("\n--- Tick %d ---\n", tick);
        process_timers_tick();
    }

    return 0;
}
