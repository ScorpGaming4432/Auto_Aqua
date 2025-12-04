#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>


inline uint8_t digitCount(uint32_t n) {
  if (n == 0) return 0;

  uint8_t count = 0;
  while (n > 0) {
    n /= 10;
    count++;
  }
  return count;
}
#endif