/*
 * Function initialize_fifo(fifo):
 *     Set up FIFO buffer
 *
 * Function write_to_fifo(fifo, data):
 *     If FIFO is not full:
 *         Add data to FIFO
 *     Else:
 *         Discard data

 * Function read_from_fifo(fifo):
 *     If FIFO is not empty:
 *         Return next data item
 *     Else:
 *         Return empty signal

 * Function get_fifo_occupancy(fifo):
 *     Return number of elements in FIFO
 */

#include "fifo.h"

#define SUCCESS 1
#define FAILURE 0


void FIFO_Init(FIFO* fifoPtr) {
	fifoPtr->head = 0;
	fifoPtr->tail = 0;
	fifoPtr->count = 0;
}

int FIFO_Write(FIFO* fifoPtr, Data value) {
    if (fifoPtr->count == fifoPtr->size) {
        return FAILURE;  // FIFO is full
    }
    fifoPtr->data[fifoPtr->head] = value;
    fifoPtr->head = (fifoPtr->head + 1) % fifoPtr->size;
    fifoPtr->count++;
    return SUCCESS;
}

int FIFO_Read(FIFO* fifoPtr, Data* value) {
    if (fifoPtr->count == 0) {
        return FAILURE;  // FIFO is empty
    }
    *value = fifoPtr->data[fifoPtr->tail];
    fifoPtr->tail = (fifoPtr->tail + 1) % fifoPtr->size;
    fifoPtr->count--;
    return SUCCESS;
}

void FIFO_Init_3Axis(FIFO3Axis* fifoPtr) {
	fifoPtr->head = 0;
	fifoPtr->tail = 0;
	fifoPtr->count = 0;
}

int FIFO_Write_3Axis(FIFO3Axis* fifoPtr, Data3Axis value) {
    if (fifoPtr->count == fifoPtr->size) {
        return FAILURE;  // FIFO is full
    }
    fifoPtr->data[fifoPtr->head] = value;
    fifoPtr->head = (fifoPtr->head + 1) % fifoPtr->size;
    fifoPtr->count++;
    return SUCCESS;
}

int FIFO_Read_3Axis(FIFO3Axis* fifoPtr, Data3Axis* value) {
    if (fifoPtr->count == 0) {
        return FAILURE;  // FIFO is empty
    }
    *value = fifoPtr->data[fifoPtr->tail];
    fifoPtr->tail = (fifoPtr->tail + 1) % fifoPtr->size;
    fifoPtr->count--;
    return SUCCESS;
}

