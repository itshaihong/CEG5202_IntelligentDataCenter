/*
 * Purpose: Initialize the system, set up FreeRTOS tasks, configure peripherals.
 * Content:
 * System initialization (clocks, GPIOs, USART, etc.).
 * Creation of FreeRTOS tasks for each sensor.
 * Initialization of FIFO buffers and scheduling strategies.
 * Start the FreeRTOS scheduler.
 */

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "sensors.h"
#include "scheduler.h"
#include "cmsis_os.h"
#include "string.h"

UART_HandleTypeDef huart1;

QueueHandle_t uartQueue;

// Define the maximum message length
#define MAX_MESSAGE_LENGTH 50

// Define the Queue Size
#define QUEUE_SIZE 20

static void USART1_UART_Init(void);
static void SystemClock_Config(void);
static void MX_RTC_Init(void);

void Error_Handler(void)
{
  while (1)
  {
  }
}

// UART Task that will handle all UART transmissions
void UART_Task(void *pvParameters) {
    char queueBuffer[MAX_MESSAGE_LENGTH];

    while (1) {
        // Wait for a message from the queue
        if (xQueueReceive(uartQueue, &queueBuffer, portMAX_DELAY) == pdPASS) {
            // Send the message via UART
            HAL_UART_Transmit(&huart1, (uint8_t*)queueBuffer, strlen(queueBuffer), 1000);
        }
    }
}

// Function for other tasks to send messages via UART
void send_uart_message(const char *message) {
    // Send message to the queue
    if (xQueueSend(uartQueue, message, portMAX_DELAY) != pdPASS) {
        // Handle queue send failure
    }
}

#define ACCEL_TASK_STACK_SIZE 512
#define GYRO_TASK_STACK_SIZE 512
#define MAG_TASK_STACK_SIZE 512
#define TEMP_TASK_STACK_SIZE 256
#define HUMID_TASK_STACK_SIZE 256
#define PRESS_TASK_STACK_SIZE 256
#define UART_TASK_STACK_SIZE 256
#define SCHDLR_TASK_STACK_SIZE 256

StaticTask_t xAccelTaskControlBlock;
StaticTask_t xGyroTaskControlBlock;
StaticTask_t xMagTaskControlBlock;
StaticTask_t xTempTaskControlBlock;
StaticTask_t xHumidTaskControlBlock;
StaticTask_t xPressTaskControlBlock;
StaticTask_t xUARTTaskControlBlock;
StaticTask_t xSchdlrTaskControlBlock;

StackType_t xAccelStack[ACCEL_TASK_STACK_SIZE];
StackType_t xGyroStack[GYRO_TASK_STACK_SIZE];
StackType_t xMagStack[MAG_TASK_STACK_SIZE];
StackType_t xTempStack[TEMP_TASK_STACK_SIZE];
StackType_t xHumidStack[HUMID_TASK_STACK_SIZE];
StackType_t xPressStack[PRESS_TASK_STACK_SIZE];
StackType_t xUARTStack[UART_TASK_STACK_SIZE];
StackType_t xSchdlrStack[SCHDLR_TASK_STACK_SIZE];

/*
 * Notes on RTC:
 * you must call HAL_RTC_GetDate() after HAL_RTC_GetTime() to unlock the values
 * * in the higher-order calendar shadow registers to ensure consistency between
 *  the time and date values.
 *  * Reading RTC current time locks the values in calendar shadow registers
 *  until current date is read.
 * https://stackoverflow.com/questions/47040039/stm32l4-rtc-hal-not-working
 */
RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;


int main(void)
{

  int status;

  HAL_Init();
  SystemInit();
  SystemClock_Config();
  USART1_UART_Init();
  MX_RTC_Init();

  osKernelInitialize();


//   Create the UART queue
  uartQueue = xQueueCreate(QUEUE_SIZE, MAX_MESSAGE_LENGTH);

  char tx_buffer[50];
  sprintf(tx_buffer, "Initializing sensors\r\n");
  HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), 1000);
  status = sensors_init();


//#define configUSE_TIME_SLICING 1


/********************************************
 * example task creation
 * -------------------------------------------
  xTaskCreateStatic(
      vAccelSensorTask,       // Task function
      "Accel Task",           // Task name
      ACCEL_TASK_STACK_SIZE,  // Stack size
      NULL,                 // Task parameters
      2,                    // Task priority
      xAccelStack,            // Stack buffer
      &xAccelTaskControlBlock // TCB buffer
  );
  *********************************************/
  initI2CMutex();
  xTaskCreateStatic(vAccelSensorTask, "Accel Task", ACCEL_TASK_STACK_SIZE, NULL, 2, xAccelStack, &xAccelTaskControlBlock);
  xTaskCreateStatic(vGyroSensorTask, "Gyro Task", GYRO_TASK_STACK_SIZE, NULL, 2, xGyroStack, &xGyroTaskControlBlock);
  xTaskCreateStatic(vMagSensorTask,  "Mag Task",  MAG_TASK_STACK_SIZE,  NULL,  2, xMagStack, &xMagTaskControlBlock);
  xTaskCreateStatic(vTempSensorTask, "Temp Task",  TEMP_TASK_STACK_SIZE, NULL, 2,  xTempStack,  &xTempTaskControlBlock);
  xTaskCreateStatic(vHumidSensorTask, "Humid Task", HUMID_TASK_STACK_SIZE, NULL, 2, xHumidStack, &xHumidTaskControlBlock);
  xTaskCreateStatic(vPressSensorTask, "Press Task", PRESS_TASK_STACK_SIZE, NULL, 2, xPressStack, &xPressTaskControlBlock);
  xTaskCreateStatic(UART_Task, "UART_Task", UART_TASK_STACK_SIZE, NULL, 1, xUARTStack, &xUARTTaskControlBlock);
  xTaskCreateStatic(vSchedulerTask, "Scheduler Task", SCHDLR_TASK_STACK_SIZE, NULL, 2, xSchdlrStack, &xSchdlrTaskControlBlock);

  vTaskStartScheduler();

  for(;;);


}




static void USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
	  Error_Handler();
  }


}



static void MX_RTC_Init(void)
{

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }


  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }


}



void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_LSE
                              |RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_I2C2
                              |RCC_PERIPHCLK_DFSDM1|RCC_PERIPHCLK_USB;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInit.Dfsdm1ClockSelection = RCC_DFSDM1CLKSOURCE_PCLK;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}
