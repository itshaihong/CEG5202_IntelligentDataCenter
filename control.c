/*
 * Function control_action(sensor_data):
 *     If temperature alert:
 *     	Increment heating/cooling system
 *     If humidity alert:
 *     	Adjust humidifier

 * Function log_event(event):
 *     Write event details to log
 *     If debugging, send information over UART

 * Function critical_event_handler():
 *     If interrupt is triggered:
  *        Bypass FIFO and send data to control center
 */
