/*
 * Function scheduler_task():
 *     While true:
 *         Select FIFO based on strategy (random, full, predictive)
 *         Read data from selected FIFO
 *         Process data and adjust control actions
 *         Handle interrupts for critical events
 *
 * Function select_fifo_random():
 *     Return a random FIFO
 *
 * Function select_fifo_full():
 *     Return FIFO with highest occupancy

 * Function select_fifo_predictive():
 *     Return FIFO most at risk of overflow
 */


#include "scheduler.h"

// Function prototypes for internal helper functions
static int select_fifo_random(SchedulingData *data);
static int select_fifo_full(SchedulingData *data);
static int select_fifo_predictive(SchedulingData *data);

// Main task function
void vSchedulerTask(void *pvParameters) {
    SchedulingData *data = (SchedulingData *)pvParameters;
    int scheme = 0; // Default scheme, should be set externally or passed in
    int selected_fifo = -1;

    while (1) {
        // Decide which scheme to use based on the scheme variable
        switch (scheme) {
            case 1:
                selected_fifo = select_fifo_random(data);
                break;
            case 2:
                selected_fifo = select_fifo_full(data);
                break;
            case 3:
                selected_fifo = select_fifo_predictive(data);
                break;
            default:
                // Handle invalid scheme values, could select a default or log an error
                selected_fifo = -1;
                break;
        }

        // Perform actions with the selected FIFO
        if (selected_fifo >= 0) {
            // Example action: Process or read from the selected FIFO
            // process_fifo(&data->fifos[selected_fifo]);
        }

        // Delay the task to allow other tasks to run (adjust delay as needed)
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 ms
    }
}

// Implementations of FIFO selection schemes

static int select_fifo_random(SchedulingData *data) {
    if (data->fifo_count <= 0) return -1;
    return rand() % data->fifo_count;
}

static int select_fifo_full(SchedulingData *data) {
    int max_occupancy = -1;
    int selected_index = -1;

    for (size_t i = 0; i < data->fifo_count; ++i) {
        if (data->fifos[i].occupancy > max_occupancy) {
            max_occupancy = data->fifos[i].occupancy;
            selected_index = i;
        }
    }
    return selected_index;
}

static int select_fifo_predictive(SchedulingData *data) {
    int earliest_full_time = INT_MAX;
    int selected_index = -1;

    for (size_t i = 0; i < data->fifo_count; ++i) {
        int space_left = data->fifos[i].max_capacity - data->fifos[i].occupancy;
        if (data->fifos[i].polling_rate > 0) {
            int time_to_full = space_left / data->fifos[i].polling_rate;
            if (time_to_full < earliest_full_time) {
                earliest_full_time = time_to_full;
                selected_index = i;
            }
        }
    }
    return selected_index;
}
