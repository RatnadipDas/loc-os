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

void *memset(void *buf, int8_t c, size_t n) {
    uint8_t *p = (uint8_t *)buf;
    while (n--) *p++ = c;
    return buf;
}

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    while (n--) *d++ = *s++;
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

size_t strlen(char *str) {
    size_t n = 0;
    char *s = str;
    while (*s++) n++;
    return n;
}

char *strrev(char *str) {
    for (int32_t start = 0, end = (int32_t)strlen(str) - 1; start < end;
         start++, end--) {
        char tmp = str[start];
        str[start] = str[end];
        str[end] = tmp;
    }

    return str;
}

char *itoa(int32_t num, char *str, size_t base) {
    // If num is zero, return "0"
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    size_t i = 0;
    bool isNegative = false;
    uint32_t magnitude = num;

    // Handle negative numbers (only for base 10)
    if (num < 0 && base == 10) {
        isNegative = true;
        magnitude = -num;
    }

    // Convert number to string representation
    while (magnitude) {
        int32_t rem = magnitude % base;
        str[i++] = rem < 10 ? rem + '0' : rem - 10 + 'a';
        magnitude /= base;
    }

    // Append negative sign if needed
    if (isNegative) str[i++] = '-';

    str[i] = '\0';

    // Reverse the string
    strrev(str);

    return str;
}

int32_t atoi(const char *str) {
    bool isNegative = 0;

    // Ignore leading white spaces
    while (*str == ' ') str++;

    // Handle optional negative sign
    if (*str == '-') {
        isNegative = 1;
        str++;
    }

    int32_t num;
    for (num = 0; *str >= '0' && *str <= '9'; str++) {
        num *= 10;
        num += *str - '0';
    }

    return isNegative ? -num : num;
}

void printf(const char *fmt, ...) {
    va_list vargs;
    va_start(vargs, fmt);

    for (const char *p = fmt; *p; p++) {
        if (*p != '%') {
            putchar(*p);
            continue;
        }

        switch (*++p) {  // Skip '%' and read the next character
            case '%':    // Print '%'
                putchar('%');
                break;
            case 'c':  // Print a single character.
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
                char buf[12];  // int32_t at max has 11 characters (including '-') and 1 more character for '\0'
                               // 2147483647 to -2147483648
                printf("%s", itoa(value, buf, 10));
                break;
            }
            case 'x': {  // Print an integer in hexadecimal.
                int32_t value = va_arg(vargs, int32_t);
                char buf[9];  // address has at max 8 characters and 1 more character for '\0'
                              // 0x00000000 to 0xffffffff
                printf("%s", itoa(value, buf, 16));
                break;
            }
            case '\0':  // '%' at the end of the format string, print '%'.
                putchar('%');
                p--;
                break;
            default:  // Print '%' and the character that follows '%'.
                putchar('%');
                putchar(*p);
        }
    }

    va_end(vargs);
}