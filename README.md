# Instructions to run the code
## Set up the project
1. Set up a new STM32 project called IntelDataCtr in STM32CubeIDE
2. Go to IntelDataCtr.ioc:
   - System Core -> SYS -> Timebase Source, select **TIM1**
   - Timers -> RTC -> tick **Activate Clock Source** and **Activate Calender**
   - Middleware -> FreeRTOS -> Interface, select **CMSIS_V2**
   - command+S save .ioc file and generate code
   - Delete the main.h generated in IntelDataCtr/Core/Inc
3. Clone this repository to your IntelDataCtr/Core/Src directory
4. Make sure you have the board's BSP package under IntelDataCtr/Drivers/BSP
5. Build and Run the project.

## Modify parameters
1. Modify senseor polling rate and threshold in ```sensor.init()``` function in **sensor.c**
2. Modify FIFO selection scheme and data logging frequency in ```vSchedulerTask()``` function in **scheduler.c**
