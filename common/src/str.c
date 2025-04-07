#include "str.h"

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

char *strcat(char *dest, const char *src) {
    char *p = dest;
    char *q = (char *)src;
    while (*p) p++;
    while ((*p++ = *q++));
    return dest;
}
