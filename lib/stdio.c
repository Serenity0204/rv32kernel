#include "stdio.h"
#include "syscall.h"

void putchar(char c)
{
    write(STDOUT, &c, 1);
}

void putstr(const char* s)
{
    int len = 0;
    while (s[len] != '\0') len++;
    write(STDOUT, (void*)s, len);
}

int getchar()
{
    char c;

    int bytes_read = read(STDIN, &c, 1);

    if (bytes_read == 1) return (unsigned char)c;
    return EOF;
}

int getline(char* buf, int max_len)
{
    int i = 0;
    int c;

    while (i < max_len - 1)
    {
        c = getchar();

        if (c == EOF) break;
        buf[i] = (char)c;
        i++;
        if (c == '\n') break;
    }
    buf[i] = '\0';

    return i;
}