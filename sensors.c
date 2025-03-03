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

#define SUCCESS 1
#define FAILURE 0

int sensors_init(){

	int status;

	status = HAL_Init();
	if(status != HAL_OK){ return FAILURE;}

	status = BSP_ACCELERO_Init();
	if(status != ACCELERO_OK){ return FAILURE;}
	accel.fifo_depth = 32;
	accel.interval = 1000;
	FIFO_Init(&accel_FIFO);


	status = BSP_GYRO_Init();
	if(status != GYRO_OK){ return FAILURE;}
	gyro.fifo_depth = 32;
	gyro.interval = 1000;
	FIFO_Init(&gyro_FIFO);

	status = BSP_MAGNETO_Init();
	if(status != MAGNETO_OK){ return FAILURE;}
	mag.fifo_depth = 32;
	mag.interval = 1000;
	FIFO_Init(&mag_FIFO);

	status = BSP_TSENSOR_Init();
	if(status != TSENSOR_OK){ return FAILURE;}
	temp.fifo_depth = 32;
	temp.interval = 1000;
	FIFO_Init(&temp_FIFO);

	status = BSP_HSENSOR_Init();
	if(status != HSENSOR_OK){ return FAILURE;}
	humid.fifo_depth = 32;
	humid.interval = 1000;
	FIFO_Init(&humid_FIFO);

	status = BSP_PSENSOR_Init();
	if(status != PSENSOR_OK){ return FAILURE;}
	press.fifo_depth = 32;
	press.interval = 1000;
	FIFO_Init(&press_FIFO);

	return SUCCESS;
}

void vAccelSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    float accel_fifo_buffer[accel.fifo_depth];
    accel_fifo.data = accel_fifo_buffer;
    for (;;) {

		float accel_data[3];
		int16_t accel_data_i16[3] = { 0 };			// array to store the x, y and z readings.
		BSP_ACCELERO_AccGetXYZ(accel_data_i16);		// read accelerometer
		// the function above returns 16 bit integers which are 100 * acceleration_in_m/s2. Converting to float to print the actual acceleration.
		accel_data[0] = (float)accel_data_i16[0] / 100.0f;
		accel_data[1] = (float)accel_data_i16[1] / 100.0f;
		accel_data[2] = (float)accel_data_i16[2] / 100.0f;


        if (!FIFO_Write(&accel_fifo, accel_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(accel.interval));
    }
}

void vGyroSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    float gyro_fifo_buffer[gyro.fifo_depth];
    gyro_fifo.data = gyro_fifo_buffer;
    for (;;) {

		float gyro_data[3];
		int16_t gyro_data_i16[3] = { 0 };
		BSP_GYRO_GetXYZ(gyro_data_i16);
		// TODO: divide by 100 or what?
		gyro_data[0] = (float)gyro_data_i16[0] / 100.0f;
		gyro_data[1] = (float)gyro_data_i16[1] / 100.0f;
		gyro_data[2] = (float)gyro_data_i16[2] / 100.0f;


        if (!FIFO_Write(&gyro_fifo, gyro_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(gyro.interval));
    }
}

void vMagSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    float mag_fifo_buffer[mag.fifo_depth];
    mag_fifo.data = mag_fifo_buffer;
    for (;;) {

		float mag_data[3];
		int16_t mag_data_i16[3] = { 0 };
		BSP_MAGNETO_GetXYZ(mag_data_i16);
		// TODO: divide by 100 or what?
		mag_data[0] = (float)mag_data_i16[0] / 100.0f;
		mag_data[1] = (float)mag_data_i16[1] / 100.0f;
		mag_data[2] = (float)mag_data_i16[2] / 100.0f;


        if (!FIFO_Write(&mag_fifo, mag_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }


        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(mag.interval));
    }
}



void vTempSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    float temp_fifo_buffer[temp.fifo_depth];
    temp_fifo.data = temp_fifo_buffer;
    for (;;) {

        float temp_data = BSP_TSENSOR_ReadTemp();

        if (!FIFO_Write(&temp_fifo, temp_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(temp.interval));
    }
}

void vHumidSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    float humid_fifo_buffer[humid.fifo_depth];
    humid_fifo.data = humid_fifo_buffer;
    for (;;) {

        float humid_data = BSP_HSENSOR_ReadHumidity();

        if (!FIFO_Write(&humid_fifo, humid_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(humid.interval));
    }
}

void vPressSensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    float press_fifo_buffer[press.fifo_depth];
    press_fifo.data = press_fifo_buffer;
    for (;;) {

        float press_data = BSP_PSENSOR_ReadPressure();

        if (!FIFO_Write(&press_fifo, press_data)) {
                    // Handle overflow, e.g., log an error or discard the oldest value
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(press.interval));
    }
}
