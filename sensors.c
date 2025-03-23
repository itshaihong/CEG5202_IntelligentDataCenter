#include "sensors.h"
#include "math.h"
#include <stdlib.h>



sensor_ctrl_data accel;
sensor_ctrl_data gyro;
sensor_ctrl_data mag;
sensor_ctrl_data temp;
sensor_ctrl_data humid;
sensor_ctrl_data press;

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

	LED_Init();
	LEDG_On();
	LEDO_Off();

	SENSOR_IO_Init();

	status = BSP_ACCELERO_Init();
	if(status != ACCELERO_OK){ return FAILURE;}
	accel.interval = 1000;
	accel.threshold_up = 11;
	accel.threshold_down = -11;
	accel_fifo.size = 32;
	FIFO_Init_3Axis(&accel_fifo);

	status = BSP_GYRO_Init();
	if(status != GYRO_OK){ return FAILURE;}
	gyro.interval = 1000;
	gyro.threshold_up = 50;
	gyro.threshold_down = -50;
	gyro_fifo.size = 32;
	FIFO_Init_3Axis(&gyro_fifo);

	status = BSP_MAGNETO_Init();
	if(status != MAGNETO_OK){ return FAILURE;}
	mag.interval = 1000;
	mag.threshold_up = 5;
	mag.threshold_down = -5;
	mag_fifo.size = 32;
	FIFO_Init_3Axis(&mag_fifo);

	status = BSP_TSENSOR_Init();
	if(status != TSENSOR_OK){ return FAILURE;}
	temp.interval = 5000;
	temp.threshold_up = 36;
	temp.threshold_down = 20;
	temp_fifo.size = 16;
	FIFO_Init(&temp_fifo);

	status = BSP_HSENSOR_Init();
	if(status != HSENSOR_OK){ return FAILURE;}
	humid.interval = 5000;
	humid.threshold_up = 100;
	humid.threshold_down = 30;
	humid_fifo.size = 16;
	FIFO_Init(&humid_fifo);

	status = BSP_PSENSOR_Init();
	if(status != PSENSOR_OK){ return FAILURE;}
	press.interval = 5000;
	press.threshold_up = 1000;
	press.threshold_down = 950;
	press_fifo.size = 16;
	FIFO_Init(&press_fifo);

	return SUCCESS;
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
    int response_delay;
    float error;
    double magnitude = 0;
    for (;;) {

    if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdTRUE) {
    	// Obtain the mg value
		BSP_ACCELERO_AccGetXYZ(accel_data_i16);		// read accelerometer

		xSemaphoreGive(xI2CMutex);

		error = (rand() % 10 - 5) / 100.0f;

		// the function above returns 16 bit integers which are 100 * acceleration_in_m/s2. Converting to float to print the actual acceleration.
		accel_data.x = ((float)accel_data_i16[0] * 9.8 / 1000.0f) * (1 + error);
		accel_data.y = ((float)accel_data_i16[1] * 9.8 / 1000.0f) * (1 + error);
		accel_data.z = ((float)accel_data_i16[2] * 9.8 / 1000.0f) * (1 + error);


		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		accel_data.milliSeconds = milliseconds;
		accel_data.Hours = sTime.Hours;
		accel_data.Minutes = sTime.Minutes;
		accel_data.Seconds = sTime.Seconds;


        // Check if data is abnormal

        if (magnitude > accel.threshold_up || magnitude < accel.threshold_down) {

    		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        	// in case abnormal data, log message and flash led
        	LEDG_Off();
        	LEDO_On();


        	sprintf(message, "%02d:%02d:%02d:%03ld Abnormal accelerometer reading\r",
        			accel_data.Hours, accel_data.Minutes, accel_data.Seconds, accel_data.milliSeconds);
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

        	sprintf(message, "Alarm!!! Abnormal vibration!!!\r");
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

            response_delay = (sTime.Hours - accel_data.Hours) * 3600000 + (sTime.Minutes - accel_data.Minutes) * 60000 +
            		(sTime.Seconds - accel_data.Seconds) * 1000 + (milliseconds - accel_data.milliSeconds);

            sprintf(message, "delay (ms): %d\r\n\r\n", response_delay);
            HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);


        }else{
        	LEDG_On();
        	LEDO_Off();
        }

        if (!FIFO_Write_3Axis(&accel_fifo, accel_data)) {
        	sprintf(message, "%02d:%02d:%02d:%03ld Accelerometer FIFO overflow\r\n\r\n",
        			accel_data.Hours, accel_data.Minutes, accel_data.Seconds, accel_data.milliSeconds);
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);
        }



    }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(accel.interval + (rand() % 10) + 10));
    }
}

void vGyroSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    Data3Axis gyro_fifo_buffer[gyro_fifo.size];
    gyro_fifo.data = gyro_fifo_buffer;

    Data3Axis gyro_data;
	float gyro_data_i16[3] = { 0 };
	char message[50];
	int response_delay;
	float error;
	double magnitude;
    for (;;) {

    	if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdTRUE) {

    	// mdps value
		BSP_GYRO_GetXYZ(gyro_data_i16);
		xSemaphoreGive(xI2CMutex);

		error = (rand() % 10 - 5) / 100.0f;

		// divide by 1000 for dps value
		gyro_data.x = ((float)gyro_data_i16[0] / 1000.0f) * (1 + error);
		gyro_data.y = ((float)gyro_data_i16[1] / 1000.0f) * (1 + error);
		gyro_data.z = ((float)gyro_data_i16[2] / 1000.0f) * (1 + error);

		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		gyro_data.milliSeconds = milliseconds;
		gyro_data.Hours = sTime.Hours;
		gyro_data.Minutes = sTime.Minutes;
		gyro_data.Seconds = sTime.Seconds;

		// TODO: Check if data is abnormal, confirm threshold values
		magnitude = sqrt(gyro_data.x * gyro_data.x + gyro_data.y * gyro_data.y + gyro_data.z * gyro_data.z);

        if (magnitude > gyro.threshold_up || magnitude < gyro.threshold_down) {

    		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        	// in case abnormal data, log message and flash led
        	LEDG_Off();
        	LEDO_On();


        	sprintf(message, "%02d:%02d:%02d:%03ld Abnormal gyroscope reading\r",
        			gyro_data.Hours, gyro_data.Minutes, gyro_data.Seconds, gyro_data.milliSeconds);
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

        	sprintf(message, "Alarm!!! Abnormal vibration!!!\r");
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

            response_delay = (sTime.Hours - gyro_data.Hours) * 3600000 + (sTime.Minutes - gyro_data.Minutes) * 60000 +
            		(sTime.Seconds - gyro_data.Seconds) * 1000 + (milliseconds - gyro_data.milliSeconds);

            sprintf(message, "delay (ms): %d\r\n\r\n", response_delay);
            HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

        }else{
        	LEDG_On();
        	LEDO_Off();
        }


        if (!FIFO_Write_3Axis(&gyro_fifo, gyro_data)) {
        	sprintf(message, "%02d:%02d:%02d:%03ld Gyroscope FIFO overflow\r\n\r\n",
        			gyro_data.Hours, gyro_data.Minutes, gyro_data.Seconds, gyro_data.milliSeconds);
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);
        }


    }

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(gyro.interval + (rand() % 20) + 10));
    }
}

void vMagSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    Data3Axis mag_fifo_buffer[mag_fifo.size];
    mag_fifo.data = mag_fifo_buffer;

    Data3Axis mag_data;
	int16_t mag_data_i16[3] = { 0 };
    char message[50];

    int response_delay;
    float error;
    double magnitude;
    for (;;) {

    	if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdTRUE) {

    	// mGauss values
		BSP_MAGNETO_GetXYZ(mag_data_i16);
		xSemaphoreGive(xI2CMutex);

		error = (rand() % 10 - 5) / 100.0f;
		// divide by 1000 for gauss value
		mag_data.x = ((float)mag_data_i16[0] / 1000.0f) * (1 + error);
		mag_data.y = ((float)mag_data_i16[1] / 1000.0f) * (1 + error);
		mag_data.z = ((float)mag_data_i16[2] / 1000.0f) * (1 + error);

		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		mag_data.milliSeconds = milliseconds;
		mag_data.Hours = sTime.Hours;
		mag_data.Minutes = sTime.Minutes;
		mag_data.Seconds = sTime.Seconds;

        // TODO: Check if data is abnormal, confirm threshold values
		magnitude = sqrt(mag_data.x * mag_data.x + mag_data.y * mag_data.y + mag_data.z * mag_data.z);

        if (magnitude > mag.threshold_up || magnitude < mag.threshold_down) {

    		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        	// in case abnormal data, log message and flash led
        	LEDG_Off();
        	LEDO_On();


        	sprintf(message, "%02d:%02d:%02d:%03ld Abnormal magnetometer reading\r",
        			mag_data.Hours, mag_data.Minutes, mag_data.Seconds, mag_data.milliSeconds);
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

        	sprintf(message, "Turning on electromagnetic protection system...\r");
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);


            response_delay = (sTime.Hours - mag_data.Hours) * 3600000 + (sTime.Minutes - mag_data.Minutes) * 60000 +
            		(sTime.Seconds - mag_data.Seconds) * 1000 + (milliseconds - mag_data.milliSeconds);

            sprintf(message, "delay (ms): %d\r\n\r\n", response_delay);
            HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);



        }else{
        	LEDG_On();
        	LEDO_Off();
        }


        if (!FIFO_Write_3Axis(&mag_fifo, mag_data)) {
        	sprintf(message, "%02d:%02d:%02d:%03ld Magnetometer FIFO overflow\r\n\r\n",
        			mag_data.Hours, mag_data.Minutes, mag_data.Seconds, mag_data.milliSeconds);
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);
        }


    }


        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(mag.interval + (rand() % 20) + 10));
    }
}



void vTempSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    Data temp_fifo_buffer[temp_fifo.size];
    temp_fifo.data = temp_fifo_buffer;

    Data temp_data;
    char message[50];

    int response_delay;
    float error;
    for (;;) {

    	if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdTRUE) {

    	error = (rand() % 10 - 5) / 100.0f;

        temp_data.value = BSP_TSENSOR_ReadTemp() * (1 + error);
        xSemaphoreGive(xI2CMutex);


        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        temp_data.milliSeconds = milliseconds;
		temp_data.Hours = sTime.Hours;
		temp_data.Minutes = sTime.Minutes;
		temp_data.Seconds = sTime.Seconds;

        //Check if data is abnormal
        if (temp_data.value > temp.threshold_up) {

    		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        	// in case abnormal data, log message and flash led
        	LEDG_Off();
        	LEDO_On();


        	sprintf(message, "%02d:%02d:%02d:%03ld Abnormal HIGH temperature reading\r",
        			temp_data.Hours, temp_data.Minutes, temp_data.Seconds, temp_data.milliSeconds);
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

        	sprintf(message, "Turning on cooling system...\r");
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

            response_delay = (sTime.Hours - temp_data.Hours) * 3600000 + (sTime.Minutes - temp_data.Minutes) * 60000 +
            		(sTime.Seconds - temp_data.Seconds) * 1000 + (milliseconds - temp_data.milliSeconds);

            sprintf(message, "delay (ms): %d\r\n\r\n", response_delay);
            HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

        }else if (temp_data.value < temp.threshold_down) {

    		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        	// in case abnormal data, log message and flash led
        	LEDG_Off();
        	LEDO_On();


        	sprintf(message, "%02d:%02d:%02d:%03ld Abnormal LOW temperature reading\r",
        			temp_data.Hours, temp_data.Minutes, temp_data.Seconds, temp_data.milliSeconds);
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

        	sprintf(message, "Turning on heating system...\r");
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

            response_delay = (sTime.Hours - temp_data.Hours) * 3600000 + (sTime.Minutes - temp_data.Minutes) * 60000 +
            		(sTime.Seconds - temp_data.Seconds) * 1000 + (milliseconds - temp_data.milliSeconds);

            sprintf(message, "delay (ms): %d\r\n\r\n", response_delay);
            HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

        }else{
        	LEDG_On();
        	LEDO_Off();
        }

        if (!FIFO_Write(&temp_fifo, temp_data)) {
        	sprintf(message, "%02d:%02d:%02d:%03ld Temperature Sensor FIFO overflow\r\n\r\n",
        			temp_data.Hours, temp_data.Minutes, temp_data.Seconds, temp_data.milliSeconds);
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);
        }

    }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(temp.interval + (rand() % 20) + 10));
    }
}

void vHumidSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    Data humid_fifo_buffer[humid_fifo.size];
    humid_fifo.data = humid_fifo_buffer;

    Data humid_data;
    char message[50];

    int response_delay;
    float error;
    for (;;) {

    	if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdTRUE) {

    	error = (rand() % 10 - 5) / 100.0f;

        humid_data.value = BSP_HSENSOR_ReadHumidity() * (1 + error);
        xSemaphoreGive(xI2CMutex);

        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        humid_data.milliSeconds = milliseconds;
		humid_data.Hours = sTime.Hours;
		humid_data.Minutes = sTime.Minutes;
		humid_data.Seconds = sTime.Seconds;


        //TODO: check threshold. Check if data is abnormal
        if (humid_data.value > humid.threshold_up) {

    		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        	// in case abnormal data, log message and flash led
        	LEDG_Off();
        	LEDO_On();


        	sprintf(message, "%02d:%02d:%02d:%03ld Abnormal HIGH humidity reading\r",
        			humid_data.Hours, humid_data.Minutes, humid_data.Seconds, humid_data.milliSeconds);
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

        	sprintf(message, "Turning on dehumidifier...\r");
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

            response_delay = (sTime.Hours - humid_data.Hours) * 3600000 + (sTime.Minutes - humid_data.Minutes) * 60000 +
            		(sTime.Seconds - humid_data.Seconds) * 1000 + (milliseconds - humid_data.milliSeconds);

            sprintf(message, "delay (ms): %d\r\n\r\n", response_delay);
            HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

        }else if (humid_data.value < humid.threshold_down) {

    		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        	// in case abnormal data, log message and flash led
        	LEDG_Off();
        	LEDO_On();


        	sprintf(message, "%02d:%02d:%02d:%03ld Abnormal LOW humidity reading\r",
        			humid_data.Hours, humid_data.Minutes, humid_data.Seconds, humid_data.milliSeconds);
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

        	sprintf(message, "Turning on humidifier...\r");
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

            response_delay = (sTime.Hours - humid_data.Hours) * 3600000 + (sTime.Minutes - humid_data.Minutes) * 60000 +
            		(sTime.Seconds - humid_data.Seconds) * 1000 + (milliseconds - humid_data.milliSeconds);

            sprintf(message, "delay (ms): %d\r\n\r\n", response_delay);
            HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

        }else{
        	LEDG_On();
        	LEDO_Off();
        }


        if (!FIFO_Write(&humid_fifo, humid_data)) {
        	sprintf(message, "%02d:%02d:%02d:%03ld Humidity Sensor FIFO overflow\r\n\r\n",
        			humid_data.Hours, humid_data.Minutes, humid_data.Seconds, humid_data.milliSeconds);
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);
        }



    }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(humid.interval + (rand() % 20) + 10));
    }
}

void vPressSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    Data press_fifo_buffer[press_fifo.size];
    press_fifo.data = press_fifo_buffer;

    Data press_data;

    char message[50];

    int response_delay;
    float error;
    for (;;) {

    	if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdTRUE) {

    	error = (rand() % 10 - 5) / 100.0f;

    	// 260 - 1260 hPa
        press_data.value = BSP_PSENSOR_ReadPressure() * (1 + error);
        xSemaphoreGive(xI2CMutex);

        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        press_data.milliSeconds = milliseconds;
		press_data.Hours = sTime.Hours;
		press_data.Minutes = sTime.Minutes;
		press_data.Seconds = sTime.Seconds;

        //TODO: check threshold. Check if data is abnormal
        if (press_data.value > press.threshold_up) {

    		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        	// in case abnormal data, log message and flash led
        	LEDG_Off();
        	LEDO_On();


        	sprintf(message, "%02d:%02d:%02d:%03ld Abnormal HIGH pressure reading\r",
        			press_data.Hours, press_data.Minutes, press_data.Seconds, press_data.milliSeconds);
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

        	sprintf(message, "Releasing pressure valve...\r");
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

            response_delay = (sTime.Hours - press_data.Hours) * 3600000 + (sTime.Minutes - press_data.Minutes) * 60000 +
            		(sTime.Seconds - press_data.Seconds) * 1000 + (milliseconds - press_data.milliSeconds);

            sprintf(message, "delay (ms): %d\r\n\r\n", response_delay);
            HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);


        }else if (press_data.value < press.threshold_down) {

    		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        	// in case abnormal data, log message and flash led
        	LEDG_Off();
        	LEDO_On();


        	sprintf(message, "%02d:%02d:%02d:%03ld Abnormal LOW pressure reading\r",
        			press_data.Hours, press_data.Minutes, press_data.Seconds, press_data.milliSeconds);
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

        	sprintf(message, "Turning on pressure pump...\r");
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

            response_delay = (sTime.Hours - press_data.Hours) * 3600000 + (sTime.Minutes - press_data.Minutes) * 60000 +
            		(sTime.Seconds - press_data.Seconds) * 1000 + (milliseconds - press_data.milliSeconds);

            sprintf(message, "delay (ms): %d\r\n\r\n", response_delay);
            HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);

        }else{
        	LEDG_On();
        	LEDO_Off();
        }


        if (!FIFO_Write(&press_fifo, press_data)) {
        	sprintf(message, "%02d:%02d:%02d:%03ld Pressure Sensor FIFO overflow\r\n\r\n",
        			press_data.Hours, press_data.Minutes, press_data.Seconds, press_data.milliSeconds);
        	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);
        }


    }


        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(press.interval + (rand() % 20) + 10));
    }
}



// LED0: green LED
// LED1: orange LED
void LED_Init(void) {
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructC = {0};
    GPIO_InitTypeDef GPIO_InitStructB = {0};

    GPIO_InitStructC.Pin = GPIO_PIN_9;
    GPIO_InitStructC.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructC.Pull = GPIO_NOPULL;
    GPIO_InitStructC.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructC);

    GPIO_InitStructB.Pin = GPIO_PIN_14;
    GPIO_InitStructB.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructB.Pull = GPIO_NOPULL;
    GPIO_InitStructB.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructB);
}

void LEDG_On(void) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
}

void LEDG_Off(void) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
}

void LEDG_Toggle(void) {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
}

void LEDO_On(void) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
}

void LEDO_Off(void) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
}

void LEDO_Toggle(void) {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
}
