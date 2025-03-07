/*
 * fifo.c
 *
 * Purpose: Implement FIFO buffer management.
 * Content:
 * Functions for FIFO initialization, reading, writing, and handling overflow.
 * Interface for FIFO selection based on different strategies.
 *
 * fifo.h
 *
 * Purpose: Declare FIFO management functions and data structures.
 * Content:
 * Function prototypes for FIFO operations.
 * Definitions of FIFO data structures.
 */


/*
 * Declare initialize_fifo(fifo)
 * Declare write_to_fifo(fifo, data)
 * Declare read_from_fifo(fifo)
 * Declare get_fifo_occupancy(fifo)
 * Define FIFO data structures
 */

#ifndef FIFO_H
#define FIFO_H

typedef struct {
    float *data;
    int head;
    int tail;
    int count;
    int size;
} FIFO;

// Function prototypes
void FIFO_Init(FIFO* fifoPtr);
int FIFO_Write(FIFO* fifoPtr, int value);
int FIFO_Read(FIFO* fifoPtr, int* value);
int FIFO_IsFull(FIFO* fifoPtr);
int FIFO_IsEmpty(FIFO* fifoPtr);

#endif
