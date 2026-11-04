/* SPDX-License-Identifier: MIT */
#ifndef E902_STRING_H
#define E902_STRING_H
#include <stddef.h>
void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *dest, int value, size_t n);
int memcmp(const void *a, const void *b, size_t n);
size_t strlen(const char *s);
#endif
