/**
 * ============================================================================
 * SENSOR.H - Water Level Sensor Interface
 * ============================================================================
 * 
 * Defines water level sensing functions using I2C protocol.
 * Supports dual-level water sensors for high and low detection.
 * 
 * I2C Connections:
 *   SDA - Purple wire (I2C Data)
 *   SCL - White wire (I2C Clock)
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

// I2C address for high-level water sensor
#define ADDR_HIGH 0x78
// I2C address for low-level water sensor
#define ADDR_LOW 0x77

/**
 * Read data from I2C water sensor
 * @param addr I2C device address
 * @param length Number of bytes to read
 * @return Sum of bytes read from the sensor
 */
int readI2CData(byte addr, int length);

/**
 * Read water level from dual-level sensors
 * Compares readings from high and low sensors to determine current level
 * @return Difference between high and low level readings
 */
uint16_t read_water_sensor();

#endif
