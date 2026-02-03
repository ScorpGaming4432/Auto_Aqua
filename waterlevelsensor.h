#include "Arduino.h"
#include <Wire.h>

class WaterLevelSensor {
  public:

  WaterLevelSensor();

  int readPercentage();
  uint32_t readWaterLevel(); //returns full uint32 range of touch sensor data
  
  uint8_t getTouchedSections(); //microcontrollers prefer the uint8_t type insted of int
  
  private:

};