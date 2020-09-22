#include <cstdio>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define isdigit(x) (x >= '0' && x <= '9')

typedef float val_t;

inline val_t scan(char **p)
{
    val_t n;
    int neg = 1;
    while (!isdigit(**p) && **p != '-' && **p != '.') ++*p;
    if (**p == '-') neg = -1, ++*p;
    for (n=0; isdigit(**p); ++*p) (n *= 10) += (**p-'0');
    if (*(*p)++ != '.') return n*neg;
    val_t d = 1;
    for (; isdigit(**p); ++*p) n += (d /= 10) * (**p-'0');
    return n*neg;
}

// with github.com/richardfeynmanrocks
void prep()
{
    FILE* wptr = fopen("compressed.bin", "wb");
    static const auto BUFFER_SIZE = 16*1024;
    int fd = open("data.csv", O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        printf("fd == -1\n");
    }
    val_t tmp;
    char buf[BUFFER_SIZE + 1];
    while (size_t bytes_read = read(fd, buf, BUFFER_SIZE))
    {
        if (bytes_read == (size_t)-1) {
            printf("bytes_read == (size_t)-1\n");
        }
        if (!bytes_read) break;
        for (char *p = buf;;) {
            char* bound = (char*) memchr(p, '\n', (buf + bytes_read) - p);
            if (bound - p < 0) break; // Stop.
            for (int i=0; i<5; ++i) {
                tmp = scan(&p);
                fwrite((void*)&tmp, sizeof(val_t), 1, wptr);
            }
            p = bound + 1;
            putc('\n', wptr);
        }
    }
}
void read()
{
    static const auto BUFFER_SIZE = 16*1024;
    int fd = open("data.csv", O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        printf("fd == -1\n");
    }
    val_t tmp;
    char buf[BUFFER_SIZE + 1];
    while (size_t bytes_read = read(fd, buf, BUFFER_SIZE))
    {
        if (bytes_read == (size_t)-1) {
            printf("bytes_read == (size_t)-1\n");
        }
        if (!bytes_read) break;
        for (char *p = buf;;) {
            char* bound = (char*) memchr(p, '\n', (buf + bytes_read) - p);
            if (bound - p < 0) break; // Stop.
            for (int i=0; i<5; ++i) {
                tmp = *(float*)p;
                printf("%f\n", tmp);
                p+=sizeof(float);
            }
            p = bound + 1;
        }
    }
}

int main()
{
    prep();
    read();
}

