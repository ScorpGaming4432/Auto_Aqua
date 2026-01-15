/**
 * ============================================================================
 * WATER.CPP - Water System Management
 * ============================================================================
 * 
 * Implements water pump control and water level sensing operations.
 * Provides functions to activate pumps and read sensor data via I2C.
*/

#include "pumps.h"
#include "sensor.h"
#include <Wire.h>
#include <Arduino.h>

/**
 * Activate a pump for a specified duration
 * Controls pump relay via digital pin to dispense precise amounts of liquid.
 * @param pump_pin Digital pin connected to pump relay (HIGH=on, LOW=off)
 * @param duration_ms Duration to keep pump running in milliseconds
 */
 void pump_work(uint8_t pump_pin, uint16_t duration_ms) {
  // Activate pump by setting pin HIGH
  digitalWrite(pump_pin, HIGH);
  // Wait for specified duration
  delay(duration_ms);
  // Deactivate pump by setting pin LOW
  digitalWrite(pump_pin, LOW);
}

/**
 * Read data from I2C water sensor device
 * Communicates with sensor at specified address and reads requested bytes.
 * @param addr I2C device address
 * @param length Number of bytes to read from device
 * @return Sum of all bytes read from the sensor
 */
 int readI2CData(byte addr, int length) {
  // Request specified number of bytes from I2C device
  Wire.requestFrom(addr, length);
  int sum = 0;
  // Sum all received bytes
  while (Wire.available()) {
    sum += Wire.read();
  }
  return sum;
}

/**
 * Read water level from dual-level sensors
 * Compares high and low level sensors to determine water level.
 * Outputs debugging information to serial console.
 * @return Difference between high and low sensor readings
 */
 uint16_t read_water_sensor() {
  // Read from upper sensor (high level detection)
  int highVal = readI2CData(ADDR_HIGH, 12);  // 12 bytes from upper section
  // Read from lower sensor (low level detection)
  int lowVal = readI2CData(ADDR_LOW, 8);     // 8 bytes from lower section

  // Debug output
  Serial.print("HighLevel: ");
  Serial.println(highVal);
  Serial.print("LowLevel : ");
  Serial.println(lowVal);

  // Return the difference as the water level indicator
  return (highVal - lowVal);
}