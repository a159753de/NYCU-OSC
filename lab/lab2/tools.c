#include "header/tools.h"

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, int n)
{
    // 逐字元比較
    while (n > 0 && *s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
        n--;
    }
    // 返回兩個不匹配字元的 ASCII 差值
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strlen(const char *s)
{
    int len = 0;
    while (*s++)
        len++;
    return len;
}

void initPath(char *str, int size)
{
    str[0] = '/';
    for (int i = 1; i < size; i++)
    {
        str[i] = '\0';
    }
}