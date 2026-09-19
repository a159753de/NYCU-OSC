#include <stdint.h>
// tools.h
#ifndef TOOLS_H
#define TOOLS_H

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int n);
int strlen(const char *s);
void initPath(char *str, int size);

// 使用 GCC 內建函數來實現 32-bit 的 byte swap
static inline uint32_t bswap32(uint32_t x)
{
    return __builtin_bswap32(x);
}

// 使用 GCC 內建函數來實現 64-bit 的 byte swap
static inline uint64_t bswap64(uint64_t x)
{
    return __builtin_bswap64(x);
}
#endif // TOOLS_H