/**
 * ============================================================================
 * WATER.H - Water System Management Interface
 * ============================================================================
 * 
 * Declares water pump control and water level sensing operations.
 * Provides functions for automatic water level management and manual control.
 */

#ifndef WATER_H
#define WATER_H

#include <stdint.h>

// Original water sensor functions
int readI2CData(byte addr, int length);
uint16_t read_water_sensor();

// Water management functions
void initWaterManagement();
void checkWaterLevel();
void toggleInletPumpMode();
void toggleOutletPumpMode();
void setInletPumpManual(bool turnOn);
void setOutletPumpManual(bool turnOn);
uint16_t getCurrentWaterLevel();
bool getInletPumpMode();
bool getOutletPumpMode();
void setWaterThresholds(int16_t low, int16_t high);
int16_t getLowThreshold();
int16_t getHighThreshold();

#endif