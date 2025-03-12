/*
 * Function initialize_sensors():
 *     Initialize HTS221 for temperature and humidity
 *     Initialize LPS22HB for pressure
 *     Initialize LSM6DSL for accelerometer and gyroscope
 *     Initialize LIS3MDL for magnetometer

 * Function read_sensor_data(sensor):
 *     Read data from specified sensor
 *     Store data in the corresponding FIFO

 * Function check_thresholds(sensor_data):
 *     If temperature > threshold or humidity < threshold:
 *     	Trigger temperature alert
 *     If pressure < threshold:
 *     	Trigger pressure alert
 *     If vibration > threshold:
 *     	Trigger vibration alert
*/

#include "sensors.h"


sensor_data accel;
sensor_data gyro;
sensor_data mag;
sensor_data temp;
sensor_data humid;
sensor_data press;

FIFO accel_fifo;
FIFO gyro_fifo;
FIFO mag_fifo;
FIFO temp_fifo;
FIFO humid_fifo;
FIFO press_fifo;

/***********************************************
 * initializing sensors, sensors params
 * and sensor FIFO
 ***********************************************/
int sensors_init(){
	int status;

	status = HAL_Init();
	if(status != HAL_OK){ return FAILURE;}

	status = BSP_ACCELERO_Init();
	if(status != ACCELERO_OK){ return FAILURE;}
	accel.interval = 1000;
	accel_fifo.size = 32;
	FIFO_Init(&accel_fifo);

	status = BSP_GYRO_Init();
	if(status != GYRO_OK){ return FAILURE;}
	gyro.interval = 1000;
	gyro_fifo.size = 32;
	FIFO_Init(&gyro_fifo);

	status = BSP_MAGNETO_Init();
	if(status != MAGNETO_OK){ return FAILURE;}
	mag.interval = 1000;
	mag_fifo.size = 32;
	FIFO_Init(&mag_fifo);

	status = BSP_TSENSOR_Init();
	if(status != TSENSOR_OK){ return FAILURE;}
	temp.interval = 1000;
	temp_fifo.size = 32;
	FIFO_Init(&temp_fifo);

	status = BSP_HSENSOR_Init();
	if(status != HSENSOR_OK){ return FAILURE;}
	humid.interval = 1000;
	humid_fifo.size = 32;
	FIFO_Init(&humid_fifo);

	status = BSP_PSENSOR_Init();
	if(status != PSENSOR_OK){ return FAILURE;}
	press.interval = 1000;
	press_fifo.size = 32;
	FIFO_Init(&press_fifo);

	return SUCCESS;
}


/***********************************************
 * Monitor sensor abnormal activity.
 * If notification received, proceed to actions
 * based on notification category.
 *
 * TODO: decide to have 1 monitor task or have
 * individual montor tasks for each sensor
 ***********************************************/
void vMonitoringTask(void *pvParameters) {
    uint32_t ulNotificationValue;

    for (;;) {
        // Wait indefinitely for a notification
        xTaskNotifyWait(
            0x00,            // Do not clear any notification bits on entry
            UINT32_MAX,       // Clear all bits on exit
            &ulNotificationValue, // Stores the notification value
            portMAX_DELAY);  // Wait indefinitely

        // Handle the notifications based on the bitmask
        if (ulNotificationValue & ACCEL_NOTIFICATION) {
            // Handle abnormal accelerometer data
        }
        if (ulNotificationValue & GYRO_NOTIFICATION) {
            // Handle abnormal gyroscope data
        }
        if (ulNotificationValue & MAG_NOTIFICATION) {
            // Handle abnormal magnetometer data
        }
        if (ulNotificationValue & TEMP_NOTIFICATION_HIGH) {
            // Handle abnormal temperature data
        }
        if (ulNotificationValue & TEMP_NOTIFICATION_LOW) {
            // Handle abnormal temperature data
        }
        if (ulNotificationValue & HUMID_NOTIFICATION_HIGH) {
            // Handle abnormal humidity data
        }
        if (ulNotificationValue & HUMID_NOTIFICATION_LOW) {
            // Handle abnormal humidity data
        }
        if (ulNotificationValue & PRESS_NOTIFICATION_HIGH) {
            // Handle abnormal pressure data
        }
        if (ulNotificationValue & PRESS_NOTIFICATION_LOW) {
            // Handle abnormal pressure data
        }
    }
}


/***********************************************
 * Sensor tasks:
 * 	poll sensors, notify monitor task if there's
 * 	abormal reading.
 ***********************************************/
void vAccelSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    float accel_fifo_buffer[accel_fifo.size];
    accel_fifo.data = accel_fifo_buffer;

    float accel_data[3];
    int16_t accel_data_i16[3] = { 0 };
    char message[50];
    for (;;) {
				// array to store the x, y and z readings.
		BSP_ACCELERO_AccGetXYZ(accel_data_i16);		// read accelerometer
		// the function above returns 16 bit integers which are 100 * acceleration_in_m/s2. Converting to float to print the actual acceleration.
		accel_data[0] = (float)accel_data_i16[0] / 100.0f;
		accel_data[1] = (float)accel_data_i16[1] / 100.0f;
		accel_data[2] = (float)accel_data_i16[2] / 100.0f;

        // Check if data is abnormal
//        if (accel_data[0] > 9.8 || accel_data[0] < -9.8 ||
//        		accel_data[1] > 9.8 || accel_data[1] < -9.8 ||
//				accel_data[2] > 9.8 || accel_data[2] < -9.8) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, ACCEL_NOTIFICATION, eSetBits);
//        }


//		  HAL_UART_Transmit(&huart1, message, sizeof(message), 1000);
		  sprintf(message, "Accel X Y Z -> %6.2f %6.2f %6.2f\r", accel_data[0], accel_data[1], accel_data[2]);
		  send_uart_message(message);

//
        if (!FIFO_Write(&accel_fifo, accel_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(accel.interval));
    }
}

void vGyroSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    float gyro_fifo_buffer[gyro_fifo.size];
    gyro_fifo.data = gyro_fifo_buffer;

	float gyro_data[3];
	float gyro_data_i16[3] = { 0 };
	char message[50];
    for (;;) {

		BSP_GYRO_GetXYZ(gyro_data_i16);
		// TODO: divide by 100 or what?
		gyro_data[0] = (float)gyro_data_i16[0] / 100.0f;
		gyro_data[1] = (float)gyro_data_i16[1] / 100.0f;
		gyro_data[2] = (float)gyro_data_i16[2] / 100.0f;

		// TODO: Check if data is abnormal, confirm threshold values
//        if (gyro_data[0] > 5 || gyro_data[0] < -5 ||
//        		gyro_data[1] > 5 || gyro_data[1] < -5 ||
//				gyro_data[2] > 5 || gyro_data[2] < -5) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, GYRO_NOTIFICATION, eSetBits);
//        }

//		HAL_UART_Transmit(&huart1, message, sizeof(message), 1000);

		  sprintf(message, "Gyro X Y Z -> %6.2f %6.2f %6.2f\r", gyro_data[0], gyro_data[1], gyro_data[2]);
		  send_uart_message(message);


        if (!FIFO_Write(&gyro_fifo, gyro_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(gyro.interval));
    }
}

void vMagSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    float mag_fifo_buffer[mag_fifo.size];
    mag_fifo.data = mag_fifo_buffer;

	float mag_data[3];
	int16_t mag_data_i16[3] = { 0 };
    char message[50];
    for (;;) {


		BSP_MAGNETO_GetXYZ(mag_data_i16);
		// TODO: divide by 100 or what?
		mag_data[0] = (float)mag_data_i16[0] / 100.0f;
		mag_data[1] = (float)mag_data_i16[1] / 100.0f;
		mag_data[2] = (float)mag_data_i16[2] / 100.0f;

        // TODO: Check if data is abnormal, confirm threshold values
//        if (mag_data[0] > 50 || mag_data[0] < -50 ||
//        		mag_data[1] > 50 || mag_data[1] < -50 ||
//				mag_data[2] > 50 || mag_data[2] < -50) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, GYRO_NOTIFICATION, eSetBits);
//        }

		  sprintf(message, "Magn X Y Z -> %6.2f %6.2f %6.2f\r", mag_data[0], mag_data[1], mag_data[2]);
		  send_uart_message(message);


        if (!FIFO_Write(&mag_fifo, mag_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }


        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(mag.interval));
    }
}



void vTempSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    float temp_fifo_buffer[temp_fifo.size];
    temp_fifo.data = temp_fifo_buffer;

    char message[20];
    for (;;) {

        float temp_data = BSP_TSENSOR_ReadTemp();

        //Check if data is abnormal
//        if (temp_data > 27) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, TEMP_NOTIFICATION_HIGH, eSetBits);
//        }
//        if (temp_data < 18) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, TEMP_NOTIFICATION_LOW, eSetBits);
//        }
		  sprintf(message, "Temp -> %6.2f\r", temp_data);
		  send_uart_message(message);


        if (!FIFO_Write(&temp_fifo, temp_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(temp.interval));
    }
}

void vHumidSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    float humid_fifo_buffer[humid_fifo.size];
    humid_fifo.data = humid_fifo_buffer;

    char message[20];
    for (;;) {

        float humid_data = BSP_HSENSOR_ReadHumidity();

        //TODO: check threshold. Check if data is abnormal
//        if (humid_data > 70) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, HUMID_NOTIFICATION_HIGH, eSetBits);
//        }
//        if (humid_data < 30) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, HUMID_NOTIFICATION_LOW, eSetBits);
//        }

        sprintf(message, "Humid -> %6.2f\r", humid_data);
        send_uart_message(message);


        if (!FIFO_Write(&humid_fifo, humid_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(humid.interval));
    }
}

void vPressSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    float press_fifo_buffer[press_fifo.size];
    press_fifo.data = press_fifo_buffer;

    char message[20];
    for (;;) {

        float press_data = BSP_PSENSOR_ReadPressure();

        //TODO: check threshold. Check if data is abnormal
//        if (press_data > 1020) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, PRESS_NOTIFICATION_HIGH, eSetBits);
//        }
//        if (press_data < 980) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, PRESS_NOTIFICATION_LOW, eSetBits);
//        }

        sprintf(message, "Press -> %6.2f\r", press_data);
        send_uart_message(message);


        if (!FIFO_Write(&press_fifo, press_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(press.interval));
    }
}
