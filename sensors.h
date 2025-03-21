/*
 * sensors.c
 *
 * Purpose: Handle sensor initialization, data acquisition, and critical threshold checks.
 * Content:
 * Initialization functions for each sensor (HTS221, LPS22HB, LSM6DSL, LIS3MDL).
 * Functions to read data from sensors and store it in FIFO buffers.
 * Functions to check data against critical thresholds and trigger interrupts if necessary.
 *
 *
 * sensors.h
 *
 * Purpose: Declare sensor-related functions and data structures.
 * Content:
 * Function prototypes for sensor initialization and data acquisition.
 * Definitions of sensor data structures and critical thresholds.
 */

/*
 * Declare initialize_sensors()
 * Declare read_sensor_data(sensor)
 * Declare check_thresholds(sensor_data)
 * Define sensor data structures and thresholds
 */

#ifndef SENSORS_H
#define SENSORS_H

#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.h"

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "fifo.h"
#include "main.h"

#define ACCEL_NOTIFICATION 		(1 << 0)
#define GYRO_NOTIFICATION  		(1 << 1)
#define MAG_NOTIFICATION   		(1 << 2)
#define TEMP_NOTIFICATION_HIGH  (1 << 3)
#define TEMP_NOTIFICATION_LOW  	(1 << 4)
#define HUMID_NOTIFICATION_HIGH (1 << 5)
#define HUMID_NOTIFICATION_LOW 	(1 << 6)
#define PRESS_NOTIFICATION_HIGH (1 << 7)
#define PRESS_NOTIFICATION_LOW 	(1 << 8)


#define SUCCESS 1
#define FAILURE 0

typedef void (*callback)(void);

typedef struct{
	int interval;
	float threshold_up;
	float threshold_down;
}sensor_ctrl_data;


extern sensor_ctrl_data accel;
extern sensor_ctrl_data gyro;
extern sensor_ctrl_data mag;
extern sensor_ctrl_data temp;
extern sensor_ctrl_data humid;
extern sensor_ctrl_data press;

extern FIFO3Axis accel_fifo;
extern FIFO3Axis gyro_fifo;
extern FIFO3Axis mag_fifo;
extern FIFO temp_fifo;
extern FIFO humid_fifo;
extern FIFO press_fifo;



int sensors_init();
void initI2CMutex();

void vAccelSensorTask(void *pvParameters);
void vGyroSensorTask(void *pvParameters);
void vMagSensorTask(void *pvParameters);
void vTempSensorTask(void *pvParameters);
void vHumidSensorTask(void *pvParameters);
void vPressSensorTask(void *pvParameters);
void vCriticalEventTask(void *pvParameters);

void LED_Init(void);
void LEDG_On(void);
void LEDG_Off(void);
void LEDG_Toggle(void);
void LEDO_On(void);
void LEDO_Off(void);
void LEDO_Toggle(void);


#endif
