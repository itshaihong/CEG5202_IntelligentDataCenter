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
#include "sensor.h"

// Function prototypes for internal helper functions
static int select_fifo_random(SchedulingData *data);
static int select_fifo_full(SchedulingData *data);
static int select_fifo_predictive(SchedulingData *data);

/* SchedulerTask select a fifo to read its data at one time;
 * selected_index:
 * 	0 -> accel_fifo
 * 	1 -> gyro_fifo
 * 	2 -> mag_fifo
 * 	3 -> temp_fifo
 * 	4 -> humid_fifo
 * 	5 -> press_fifo
 * */
void vSchedulerTask(void *pvParameters) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
	TickType_t SchedulerInterval = 1000;

    int scheme = 0; // Default scheme, should be set externally or passed in
    int selected_fifo = -1;

    char message[50];
    int *data;

    for(;;) {
        // Decide which scheme to use based on the scheme variable
        switch (scheme) {
            case 1:
                selected_fifo = select_fifo_random();
                break;
            case 2:
                selected_fifo = select_fifo_full();
                break;
            case 3:
                selected_fifo = select_fifo_predictive();
                break;
            default:
                // Handle invalid scheme values, could select a default or log an error
                selected_fifo = -1;
                break;
        }

        // Perform actions with the selected FIFO
        switch(selected_fifo){
			case 0:
				if(FIFO_IsEmpty(&accel_fifo)){
					sprintf(message, "Accel FIFO empty!\r");
					send_uart_message(message);
				}
				else{
					FIFO_Read(&accel_fifo, data);
				}
				break;

			case 1:
				if(FIFO_IsEmpty(&gyro_fifo)){
					sprintf(message, "Gyro FIFO empty!\r");
					send_uart_message(message);
				}else{
					FIFO_Read(&gyro_fifo, data);
				}
				break;

			case 2:
				if(FIFO_IsEmpty(&mag_fifo)){
					sprintf(message, "Mag FIFO empty!\r");
					send_uart_message(message);
				}else{
					FIFO_Read(&mag_fifo, data);
				}
				break;

			case 3:
				if(FIFO_IsEmpty(&temp_fifo)){
					sprintf(message, "Temp FIFO empty!\r");
					send_uart_message(message);
				}else{
					FIFO_Read(&temp_fifo, data);
				}
				break;

			case 4:
				if(FIFO_IsEmpty(&humid_fifo)){
					sprintf(message, "Humid FIFO empty!\r");
					send_uart_message(message);
				}else{
					FIFO_Read(&humid_fifo, data);
				}
				break;

			case 5:
				if(FIFO_IsEmpty(&press_fifo)){
					sprintf(message, "Press FIFO empty!\r");
					send_uart_message(message);
				}else{
					FIFO_Read(&press_fifo, data);
				}
				break;



        }



        // Delay the task to allow other tasks to run
        vTaskDelayUntil(&xLastWakeTime, SchedulerInterval);
    }
}


/* Random selection scheme: generate a random number from 0 to 5
 *
 */
static int select_fifo_random() {
    return rand() % 6;
}

/* Full selection scheme:
 * 	select the fifo that is least empty (most full)
 * 	return a number from 0 to 5
 * */
static int select_fifo_full() {

    int emptiness[6];

    emptiness[0] = accel_fifo.size - accel_fifo.count;
    emptiness[1] = gyro_fifo.size - gyro_fifo.count;
    emptiness[2] = mag_fifo.size - mag_fifo.count;
    emptiness[3] = temp_fifo.size - temp_fifo.count;
    emptiness[4] = humid_fifo.size - humid_fifo.count;
    emptiness[5] = press_fifo.size - press_fifo.count;

    int min_emptines = emptiness[0];
    int selected_index = 0;

    for (int i = 1; i < 6; i++){
    	if(emptiness[i] < min_emptiness){
    		min_emptiness = emptiness[i];
    		selected_index = i;
    	}
    }

    return selected_index;
}

/* Predictive selection scheme:
 * 	select the fifo that is going to full the earliest
 * 	return a number from 0 to 5
 * */
static int select_fifo_predictive() {
    int time_to_full[6];

    time_to_full[0] = (accel_fifo.size - accel_fifo.count) * accel.interval;
    time_to_full[1] = (gyro_fifo.size - gyro_fifo.count) * gyro.interval;
    time_to_full[2] = (mag_fifo.size - mag_fifo.count) * mag.interval;
    time_to_full[3] = (temp_fifo.size - temp_fifo.count) * temp_interval;
    time_to_full[4] = (humid_fifo.size - humid_fifo.count) * humid.interval;
    time_to_full[5] = (press_fifo.size - press_fifo.count) * press.interval;

    int min_time_to_full = time_to_full[0];
    int selected_index = 0;

    for (int i = 1; i < 6; i++){
    	if(time_to_full[i] < min_time_to_full){
    		min_time_to_full = time_to_full[i];
    		selected_index = i;
    	}
    }

    return selected_index;
}
