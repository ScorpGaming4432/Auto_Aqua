#ifndef DISPLAY_H
#define DISPLAY_H

#pragma once
#include <LiquidCrystal_I2C.h>
#include <stdint.h>

extern LiquidCrystal_I2C* lcd;
extern uint32_t dimTimer;

bool findLCD();
// (Previously provided helpers removed - use `lcd->print()` / `lcd->write()`.)
void updateBacklight();
//bool initDisplay(); // optional convenience function

#endif

