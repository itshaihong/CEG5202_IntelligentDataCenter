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

#include "stdio.h"
#include "task.h"
#include "fifo.h"

typedef void (*callback)(void);

typedef struct{
	int fifo_depth;
	int interval;
	callback interrupt_handler;
}sensor_data;


sensor_data accel;
sensor_data gyro;
sensor_data mag;
sensor_data temp;
sensor_data humid;
sensor_data press;

FIFO accel_fifo;
FIFO gyro_fifo;
FIFO mag_FIFO;
FIFO temp_fifo;
FIFO humid_fifo;
FIFO press_fifo;



int sensors_init();
int sensors_polling();

#endif
