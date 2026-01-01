#include "stdio.h"
#include "syscall.h"

void putchar(char c)
{
    write(1, &c, 1);
}

void putstr(const char* s)
{
    int len = 0;
    while (s[len] != '\0') len++;
    write(1, (void*)s, len);
}