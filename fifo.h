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

#include "main.h"


typedef struct {
	uint8_t Hours;
	uint8_t Minutes;
	uint8_t Seconds;
    float value;
} Data;

typedef struct {
	uint8_t Hours;
	uint8_t Minutes;
	uint8_t Seconds;
    float x;
    float y;
    float z;
} Data3Axis;

typedef struct {
	Data *data;
    int head;
    int tail;
    int count;
    int size;
} FIFO;

typedef struct {
	Data3Axis *data;
    int head;
    int tail;
    int count;
    int size;
} FIFO3Axis;

// Function prototypes
void FIFO_Init(FIFO* fifoPtr);
int FIFO_Write(FIFO* fifoPtr, Data value);
int FIFO_Read(FIFO* fifoPtr, Data* value);

void FIFO_Init_3Axis(FIFO3Axis* fifoPtr);
int FIFO_Write_3Axis(FIFO3Axis* fifoPtr, Data3Axis value);
int FIFO_Read_3Axis(FIFO3Axis* fifoPtr, Data3Axis* value);

#endif
