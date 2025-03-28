#include "lib.h"

#include "types.h"

/**
 * @brief Outputs a single character to the console.
 *
 * This function is responsible for printing a single character to the
 * standard output, typically a terminal or serial console.
 *
 * @param[in] ch The character to be printed.
 *
 * @note The actual implementation of this function will be provided
 *       either by the kernel or by the user.
 *
 * @return None.
 *
 * @example
 * @code
 * putchar('A'); // Prints 'A' to the console
 * putchar('\n'); // Prints a newline character
 * @endcode
 */
void putchar(char ch);

void *memset(void *buf, int8_t value, size_t size) {
  uint8_t *p = (uint8_t *)buf;
  while (size--) *p++ = value;
  return buf;
}

void *memcpy(void *dst, const void *src, size_t size) {
  uint8_t *d = (uint8_t *)dst;
  const uint8_t *s = (const uint8_t *)src;
  while (size--) *d++ = *s++;
  return dst;
}

char *strcpy(char *dst, const char *src) {
  char *d = dst;
  while (*src) *d++ = *src++;
  *d = '\0';
  return dst;
}

int strcmp(const char *str1, const char *str2) {
  while (*str1 && *str2) {
    if (*str1 != *str2) break;
    str1++;
    str2++;
  }

  return *(unsigned char *)str1 - *(unsigned char *)str2;
}

void printf(const char *fmt, ...) {
  va_list vargs;
  va_start(vargs, fmt);

  while (*fmt) {
    if (*fmt == '%') {
      switch (*++fmt) {  // Skip '%' and read the next character
        case '\0':       // '%' at the end of the format string
          putchar('%');
          goto end;
        case '%':  // Print '%'
          putchar('%');
          break;
        case 'c':
          char ch = va_arg(vargs, char);
          putchar(ch);
          break;
        case 's': {  // Print a NULL-terminated string.
          const char *s = va_arg(vargs, const char *);
          while (*s) {
            putchar(*s);
            s++;
          }
          break;
        }
        case 'd': {  // Print an integer in decimal.
          int32_t value = va_arg(vargs, int32_t);
          uint32_t magnitude = value;
          if (value < 0) {
            putchar('-');
            magnitude = -magnitude;
          }

          uint32_t divisor = 1;
          while (magnitude / divisor > 9) divisor *= 10;

          while (divisor > 0) {
            putchar('0' + magnitude / divisor);
            magnitude %= divisor;
            divisor /= 10;
          }

          break;
        }
        case 'x': {  // Print an integer in hexadecimal.
          uint32_t value = va_arg(vargs, uint32_t);
          for (size_t i = 0; i < 2; i++) {
            putchar("0x"[i]);
          }

          for (int32_t i = 7; i >= 0; i--) {
            uint32_t nibble = (value >> (i * 4)) & 0xf;
            putchar("0123456789abcdef"[nibble]);
          }
        }
      }
    } else {
      putchar(*fmt);
    }

    fmt++;
  }

end:
  va_end(vargs);
}