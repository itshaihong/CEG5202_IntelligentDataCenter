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

FIFO3Axis accel_fifo;
FIFO3Axis gyro_fifo;
FIFO3Axis mag_fifo;
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
	FIFO_Init_3Axis(&accel_fifo);

	status = BSP_GYRO_Init();
	if(status != GYRO_OK){ return FAILURE;}
	gyro.interval = 5000;
	gyro_fifo.size = 32;
	FIFO_Init_3Axis(&gyro_fifo);

	status = BSP_MAGNETO_Init();
	if(status != MAGNETO_OK){ return FAILURE;}
	mag.interval = 5000;
	mag_fifo.size = 32;
	FIFO_Init_3Axis(&mag_fifo);

	status = BSP_TSENSOR_Init();
	if(status != TSENSOR_OK){ return FAILURE;}
	temp.interval = 5000;
	temp_fifo.size = 32;
	FIFO_Init(&temp_fifo);

	status = BSP_HSENSOR_Init();
	if(status != HSENSOR_OK){ return FAILURE;}
	humid.interval = 5000;
	humid_fifo.size = 32;
	FIFO_Init(&humid_fifo);

	status = BSP_PSENSOR_Init();
	if(status != PSENSOR_OK){ return FAILURE;}
	press.interval = 5000;
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


SemaphoreHandle_t xI2CMutex;

void initI2CMutex() {
    xI2CMutex = xSemaphoreCreateMutex();
    if (xI2CMutex == NULL) {
        // Handle error: Failed to create the mutex
    }
}


/***********************************************
 * Sensor tasks:
 * 	poll sensors, notify monitor task if there's
 * 	abormal reading.
 ***********************************************/
void vAccelSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    Data3Axis accel_fifo_buffer[accel_fifo.size];
    accel_fifo.data = accel_fifo_buffer;

    Data3Axis accel_data;
    int16_t accel_data_i16[3] = { 0 };
    char message[50];
    for (;;) {

    if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdTRUE) {
				// array to store the x, y and z readings.
		BSP_ACCELERO_AccGetXYZ(accel_data_i16);		// read accelerometer
		// the function above returns 16 bit integers which are 100 * acceleration_in_m/s2. Converting to float to print the actual acceleration.
		accel_data.x = (float)accel_data_i16[0] / 100.0f;
		accel_data.y = (float)accel_data_i16[1] / 100.0f;
		accel_data.z = (float)accel_data_i16[2] / 100.0f;


		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		accel_data.Hours = sTime.Hours;
		accel_data.Minutes = sTime.Minutes;
		accel_data.Seconds = sTime.Seconds;

        // Check if data is abnormal
//        if (accel_data[0] > 9.8 || accel_data[0] < -9.8 ||
//        		accel_data[1] > 9.8 || accel_data[1] < -9.8 ||
//				accel_data[2] > 9.8 || accel_data[2] < -9.8) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, ACCEL_NOTIFICATION, eSetBits);
//        }


//		sprintf(message, "%02d:%02d:%02d Accel X Y Z -> %6.2f %6.2f %6.2f\r",
//				accel_data.Hours, accel_data.Minutes, accel_data.Seconds, accel_data.x, accel_data.y, accel_data.z);
//		  send_uart_message(message);



//
        if (!FIFO_Write_3Axis(&accel_fifo, accel_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }

        xSemaphoreGive(xI2CMutex);
    }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(accel.interval));
    }
}

void vGyroSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    Data3Axis gyro_fifo_buffer[gyro_fifo.size];
    gyro_fifo.data = gyro_fifo_buffer;

    Data3Axis gyro_data;
	float gyro_data_i16[3] = { 0 };
	char message[50];
    for (;;) {

    	if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdTRUE) {

		BSP_GYRO_GetXYZ(gyro_data_i16);
		// TODO: divide by 100 or what?
		gyro_data.x = (float)gyro_data_i16[0] / 100.0f;
		gyro_data.y = (float)gyro_data_i16[1] / 100.0f;
		gyro_data.z = (float)gyro_data_i16[2] / 100.0f;

		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		gyro_data.Hours = sTime.Hours;
		gyro_data.Minutes = sTime.Minutes;
		gyro_data.Seconds = sTime.Seconds;

		// TODO: Check if data is abnormal, confirm threshold values
//        if (gyro_data[0] > 5 || gyro_data[0] < -5 ||
//        		gyro_data[1] > 5 || gyro_data[1] < -5 ||
//				gyro_data[2] > 5 || gyro_data[2] < -5) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, GYRO_NOTIFICATION, eSetBits);
//        }


//		  sprintf(message, "%02d:%02d:%02d Gyro X Y Z -> %6.2f %6.2f %6.2f\r",
//				  gyro_data.Hours, gyro_data.Minutes, gyro_data.Seconds, gyro_data.x, gyro_data.y, gyro_data.z);
//		  send_uart_message(message);


        if (!FIFO_Write_3Axis(&gyro_fifo, gyro_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }

        xSemaphoreGive(xI2CMutex);
    }

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(gyro.interval));
    }
}

void vMagSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    Data3Axis mag_fifo_buffer[mag_fifo.size];
    mag_fifo.data = mag_fifo_buffer;

    Data3Axis mag_data;
	int16_t mag_data_i16[3] = { 0 };
    char message[50];
    for (;;) {

    	if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdTRUE) {


		BSP_MAGNETO_GetXYZ(mag_data_i16);
		// TODO: divide by 100 or what?
		mag_data.x = (float)mag_data_i16[0] / 100.0f;
		mag_data.y = (float)mag_data_i16[1] / 100.0f;
		mag_data.z = (float)mag_data_i16[2] / 100.0f;

		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		mag_data.Hours = sTime.Hours;
		mag_data.Minutes = sTime.Minutes;
		mag_data.Seconds = sTime.Seconds;

        // TODO: Check if data is abnormal, confirm threshold values
//        if (mag_data[0] > 50 || mag_data[0] < -50 ||
//        		mag_data[1] > 50 || mag_data[1] < -50 ||
//				mag_data[2] > 50 || mag_data[2] < -50) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, GYRO_NOTIFICATION, eSetBits);
//        }

//		  sprintf(message, "%02d:%02d:%02d Magn X Y Z -> %6.2f %6.2f %6.2f\r",
//				  mag_data.Hours, mag_data.Minutes, mag_data.Seconds, mag_data.x, mag_data.y, mag_data.z);
//		  send_uart_message(message);


        if (!FIFO_Write_3Axis(&mag_fifo, mag_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }

        xSemaphoreGive(xI2CMutex);
    }


        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(mag.interval));
    }
}



void vTempSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    Data temp_fifo_buffer[temp_fifo.size];
    temp_fifo.data = temp_fifo_buffer;

    Data temp_data;
    char message[50];
    for (;;) {

    	if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdTRUE) {

        temp_data.value = BSP_TSENSOR_ReadTemp();

        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		temp_data.Hours = sTime.Hours;
		temp_data.Minutes = sTime.Minutes;
		temp_data.Seconds = sTime.Seconds;

        //Check if data is abnormal
//        if (temp_data > 27) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, TEMP_NOTIFICATION_HIGH, eSetBits);
//        }
//        if (temp_data < 18) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, TEMP_NOTIFICATION_LOW, eSetBits);
//        }


//		  sprintf(message, "%02d:%02d:%02d Temp -> %6.2f\r",
//				  temp_data.Hours, temp_data.Minutes, temp_data.Seconds, temp_data.value);
//		  send_uart_message(message);


        if (!FIFO_Write(&temp_fifo, temp_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }


        xSemaphoreGive(xI2CMutex);
    }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(temp.interval));
    }
}

void vHumidSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    Data humid_fifo_buffer[humid_fifo.size];
    humid_fifo.data = humid_fifo_buffer;

    Data humid_data;
    char message[50];
    for (;;) {

    	if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdTRUE) {

        humid_data.value = BSP_HSENSOR_ReadHumidity();

        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		humid_data.Hours = sTime.Hours;
		humid_data.Minutes = sTime.Minutes;
		humid_data.Seconds = sTime.Seconds;


        //TODO: check threshold. Check if data is abnormal
//        if (humid_data > 70) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, HUMID_NOTIFICATION_HIGH, eSetBits);
//        }
//        if (humid_data < 30) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, HUMID_NOTIFICATION_LOW, eSetBits);
//        }

//        sprintf(message, "%02d:%02d:%02d Humid -> %6.2f\r",
//        		humid_data.Hours, humid_data.Minutes, humid_data.Seconds, humid_data.value);
//        send_uart_message(message);


        if (!FIFO_Write(&humid_fifo, humid_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }


        xSemaphoreGive(xI2CMutex);
    }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(humid.interval));
    }
}

void vPressSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    Data press_fifo_buffer[press_fifo.size];
    press_fifo.data = press_fifo_buffer;

    Data press_data;

    char message[50];
    for (;;) {

    	if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdTRUE) {

        press_data.value = BSP_PSENSOR_ReadPressure();

        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		press_data.Hours = sTime.Hours;
		press_data.Minutes = sTime.Minutes;
		press_data.Seconds = sTime.Seconds;

        //TODO: check threshold. Check if data is abnormal
//        if (press_data > 1020) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, PRESS_NOTIFICATION_HIGH, eSetBits);
//        }
//        if (press_data < 980) {
//            // Notify the monitoring task with the unique bitmask
//            xTaskNotify(vMonitoringTask, PRESS_NOTIFICATION_LOW, eSetBits);
//        }

//        sprintf(message, "%02d:%02d:%02d Press -> %6.2f\r",
//        		press_data.Hours, press_data.Minutes, press_data.Seconds, press_data.value);
//        send_uart_message(message);


        if (!FIFO_Write(&press_fifo, press_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }

        xSemaphoreGive(xI2CMutex);
    }


        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(press.interval));
    }
}
