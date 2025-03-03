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
