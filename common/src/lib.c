#include "lib.h"

#include "types.h"

void *memset(void *buf, int8_t value, size_t size) {
  uint8_t *p = (uint8_t *)buf;
  while (size--) *p++ = value;
  return buf;
}