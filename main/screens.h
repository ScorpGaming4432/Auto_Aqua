// Declarations for screen functions used by main.ino
#ifndef SCREENS_H
#define SCREENS_H

#include <stdint.h>
#include "language.h"

void splashScreen();
uint8_t langConfigScreen(uint8_t idx);
uint32_t tankVolumeScreen(const char* tankVolumeBuf);
void lcdPrintWithGlyphs(const char* str, uint8_t length);

#endif
