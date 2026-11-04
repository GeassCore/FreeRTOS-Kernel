/* SPDX-License-Identifier: MIT */
#include <string.h>
void *memcpy(void *dest, const void *src, size_t n)
{
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (n--) *d++ = *s++;
    return dest;
}
void *memset(void *dest, int value, size_t n)
{
    unsigned char *d = dest;
    while (n--) *d++ = (unsigned char)value;
    return dest;
}
int memcmp(const void *a, const void *b, size_t n)
{
    const unsigned char *p = a, *q = b;
    while (n--) {
        if (*p != *q) return (int)*p - (int)*q;
        p++; q++;
    }
    return 0;
}
size_t strlen(const char *s)
{
    const char *end = s;
    while (*end) end++;
    return end - s;
}
