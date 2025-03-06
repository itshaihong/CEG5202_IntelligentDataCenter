/*
 * Purpose: Initialize the system, set up FreeRTOS tasks, configure peripherals.
 * Content:
 * System initialization (clocks, GPIOs, USART, etc.).
 * Creation of FreeRTOS tasks for each sensor.
 * Initialization of FIFO buffers and scheduling strategies.
 * Start the FreeRTOS scheduler.
 */

/*
 * Initialize system clocks and peripherals
 * Initialize UART for debugging
 * Create FIFO buffers for each sensor
 * Initialize sensors
 * Create FreeRTOS tasks for:
 *     - Sensor data acquisition
 *     - Scheduler management
 *     - Critical event handling
 * Start the FreeRTOS scheduler
 */



/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sensors.h"


int main(void)
{

  HAL_Init();

//  xTaskCreate(vAccelSensorTask, "Accel Task", 1000, NULL, 1, NULL);
//  xTaskCreate(vGyroSensorTask, "Gyro Task", 1000, NULL, 1, NULL);
//  xTaskCreate(vMagSensorTask, "Mag Task", 1000, NULL, 1, NULL);
//  xTaskCreate(vTempSensorTask, "Temp Task", 1000, NULL, 1, NULL);
//  xTaskCreate(vHumidSensorTask, "Humid Task", 1000, NULL, 1, NULL);
//  xTaskCreate(vPressSensorTask, "Press Task", 1000, NULL, 1, NULL);

  vTaskStartScheduler();

  while(1){

  }


}

