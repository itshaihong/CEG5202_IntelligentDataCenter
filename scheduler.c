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
#include "sensors.h"

// Function prototypes for internal helper functions
static int select_fifo_random();
static int select_fifo_full();
static int select_fifo_predictive();

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

    int scheme = 2; // Default scheme, should be set externally or passed in
    int selected_fifo = -1;

    char message[50];
    Data data;
    Data3Axis data3Axis;

    switch (scheme) {
        case 0:
        	sprintf(message, "Random Selection Scheme!\r\n\n");
        	send_uart_message(message);
            break;
        case 1:
        	sprintf(message, "Full Selection Scheme!\r\n\n");
        	send_uart_message(message);
            break;
        case 2:
        	sprintf(message, "Predictive Selection Scheme!\r\n\n");
        	send_uart_message(message);
            break;
        default:
            // Handle invalid scheme values, could select a default or log an error
            selected_fifo = -1;
            break;
    }

    for(;;) {
        // Decide which scheme to use based on the scheme variable
        switch (scheme) {
            case 0:
                selected_fifo = select_fifo_random();
                break;
            case 1:
                selected_fifo = select_fifo_full();
                break;
            case 2:
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
				if(accel_fifo.count == 0){
					sprintf(message, "Accel FIFO empty!\r");
					send_uart_message(message);
				}
				else{
					FIFO_Read_3Axis(&accel_fifo, &data3Axis);
					sprintf(message, "%02d:%02d:%02d Accel X Y Z -> %6.2f %6.2f %6.2f\r",
							data3Axis.Hours, data3Axis.Minutes, data3Axis.Seconds,
							data3Axis.x, data3Axis.y, data3Axis.z);
					send_uart_message(message);
				}
				break;

			case 1:
				if(gyro_fifo.count == 0){
					sprintf(message, "Gyro FIFO empty!\r");
					send_uart_message(message);
				}else{
					FIFO_Read_3Axis(&gyro_fifo, &data3Axis);
					sprintf(message, "%02d:%02d:%02d Gyro X Y Z -> %6.2f %6.2f %6.2f\r",
							data3Axis.Hours, data3Axis.Minutes, data3Axis.Seconds,
							data3Axis.x, data3Axis.y, data3Axis.z);
					send_uart_message(message);
				}
				break;

			case 2:
				if(mag_fifo.count == 0){
					sprintf(message, "Mag FIFO empty!\r");
					send_uart_message(message);
				}else{
					FIFO_Read_3Axis(&mag_fifo, &data3Axis);
					sprintf(message, "%02d:%02d:%02d Mag X Y Z -> %6.2f %6.2f %6.2f\r",
							data3Axis.Hours, data3Axis.Minutes, data3Axis.Seconds,
							data3Axis.x, data3Axis.y, data3Axis.z);
					send_uart_message(message);
				}
				break;

			case 3:
				if(temp_fifo.count == 0){
					sprintf(message, "Temp FIFO empty!\r");
					send_uart_message(message);
				}else{
					FIFO_Read(&temp_fifo, &data);
					sprintf(message, "%02d:%02d:%02d Temp -> %6.2f\r",
							data.Hours, data.Minutes, data.Seconds, data.value);
					send_uart_message(message);
				}
				break;

			case 4:
				if(humid_fifo.count == 0){
					sprintf(message, "Humid FIFO empty!\r");
					send_uart_message(message);
				}else{
					FIFO_Read(&humid_fifo, &data);
					sprintf(message, "%02d:%02d:%02d Humid -> %6.2f\r",
							data.Hours, data.Minutes, data.Seconds, data.value);
					send_uart_message(message);
				}
				break;

			case 5:
				if(press_fifo.count == 0){
					sprintf(message, "Press FIFO empty!\r");
					send_uart_message(message);
				}else{
					FIFO_Read(&press_fifo, &data);
					sprintf(message, "%02d:%02d:%02d Press -> %6.2f\r",
							data.Hours, data.Minutes, data.Seconds, data.value);
					send_uart_message(message);
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

    int min_emptiness = emptiness[0];
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
    time_to_full[3] = (temp_fifo.size - temp_fifo.count) * temp.interval;
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
