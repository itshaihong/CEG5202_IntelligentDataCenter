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
