/*
 * scheduler.c
 *
 * Purpose: Implement scheduling strategies and real-time task management.
 * Content:
 * Implementation of random, full buffer, and predictive FIFO selection strategies.
 * FreeRTOS task definitions for sensor polling and critical event handling.
 * Logic for adaptive response and control actions based on sensor data.
 *
 *
 * scheduler.h
 *
 * Purpose: Declare scheduling-related functions and data structures.
 * Content:
 * Function prototypes for scheduling strategies.
 * Definitions of task handles and related data structures.
 */

/*
 * Declare scheduler_task()
 * Declare select_fifo_random()
 * Declare select_fifo_full()
 * Declare select_fifo_predictive()
 * Declare Define task handles and scheduling data structures
 */

void vSchedulerTask(void *pvParameters);

