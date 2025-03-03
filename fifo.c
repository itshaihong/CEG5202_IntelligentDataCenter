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

void FIFO_Init(FIFO* fifoPtr) {
	fifoPtr->head = 0;
	fifoPtr->tail = 0;
	fifoPtr->count = 0;
}

int FIFO_Write(FIFO* fifoPtr, int value) {
    if (FIFO_IsFull(fifoPtr)) {
        return false;  // FIFO is full
    }
    fifoPtr->data[fifoPtr->head] = value;
    fifoPtr->head = (fifoPtr->head + 1) % FIFO_SIZE;
    fifoPtr->count++;
    return true;
}

int FIFO_Read(FIFO* fifoPtr, int* value) {
    if (FIFO_IsEmpty(fifoPtr)) {
        return false;  // FIFO is empty
    }
    *value = fifoPtr->data[fifoPtr->tail];
    fifoPtr->tail = (fifoPtr->tail + 1) % FIFO_SIZE;
    fifoPtr->count--;
    return true;
}

int FIFO_IsFull(FIFO* fifoPtr) {
    return fifoPtr->count == FIFO_SIZE;
}

int FIFO_IsEmpty(FIFO* fifoPtr) {
    return fifoPtr->count == 0;
}
