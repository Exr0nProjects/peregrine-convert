#include <cstdio>
#include <chrono>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define isdigit(x) (x >= '0' && x <= '9')
#define isnumchar(x) (isdigit(x) || x == '-' || x == '.')
#define tonum(x) (x-'0')    // should convert anything that isnumchar() and isn't '-' or '.'
                            // could be replaced with a hex conversion expression, for example

const size_t  IN_BUFFER_SIZE = 16*1024;
const size_t OUT_BUFFER_SIZE = 16;

const size_t DEBUG_BUF_SIZE = 1e5;
float proc[DEBUG_BUF_SIZE];

typedef float val_t;

size_t compress()
{
    FILE* wf = fopen("compressed.bin", "wb");
    int rfd = open("data2.csv", O_RDONLY | O_NONBLOCK);
    size_t written = 0;

    if (~rfd) printf("reading file...\n");
    else { printf("Invalid input file\n"); return 0; }

    char  ibuf[ IN_BUFFER_SIZE*sizeof(char)];
    val_t obuf[OUT_BUFFER_SIZE*sizeof(val_t)];
    obuf[0] = 0;

    val_t *op = obuf; val_t neg = 1, dec = 0;
    while (size_t bytes_read = read(rfd, ibuf, IN_BUFFER_SIZE-1))
    {
        for (char *ip=ibuf; ip<ibuf+bytes_read; ++ip)
        {
            //printf("    buf[%6d]: char '%d'    cur %10.2f     neg %2.0f dec %8.8f\n", ip-ibuf, *ip, neg, dec);
            if (!isnumchar(*ip))
            {
                if (dec) // was processing a number
                    if (++op > obuf+OUT_BUFFER_SIZE)
                    {
                        written += fwrite((void*)obuf, sizeof(val_t), OUT_BUFFER_SIZE, wf);
                        op = obuf;
                    }
                *op = 0, neg = 1, dec = 0;
            }
            else if (*ip == '-') neg = -1;
            else if (*ip == '.') dec =  1;
            else    // it's a digit
            {
                if (dec > 0) *op += (dec /= 10) * tonum(*ip);
                else        (*op *= 10) += tonum(*ip), dec=-1;
            }
        }
    }
    return written;
}

size_t compress2()
{
    FILE* wf = fopen("compressed.bin", "wb");
    int rfd = open("data2.csv", O_RDONLY | O_NONBLOCK);
    size_t written = 0;

    if (~rfd) printf("reading file...\n");
    else { printf("Invalid input file\n"); return 0; }

    char  ibuf[ IN_BUFFER_SIZE*sizeof(char)];
    val_t obuf[OUT_BUFFER_SIZE*sizeof(val_t)];
    obuf[0] = 0;

    val_t *op = obuf; val_t neg = 1, dec = 0;
    while (size_t bytes_read = read(rfd, ibuf, IN_BUFFER_SIZE-1))
    {
        for (char *ip=ibuf; ip<ibuf+bytes_read; ++ip)
        {
            //printf("    buf[%6d]: char '%d'    cur %10.2f     neg %2.0f dec %8.8f\n", ip-ibuf, *ip, neg, dec);
            if (!isnumchar(*ip))
            {
                if (dec) // was processing a number
                    if (++op > obuf+OUT_BUFFER_SIZE)
                    {
                        written += fwrite((void*)obuf, sizeof(val_t), OUT_BUFFER_SIZE, wf);
                        op = obuf;
                    }
                *op = 0, neg = 1, dec = 0;
            }
            else if (*ip == '-') neg = -1;
            else if (*ip == '.') dec =  1;
            else    // it's a digit
            {
                if (dec > 0) *op += (dec /= 10) * tonum(*ip);
                else        (*op *= 10) += tonum(*ip), dec=-1;
            }
        }
    }
    return written;
}

// fancy function passing: https://stackoverflow.com/a/15959458
typedef double Sec;
template <class F>
Sec stopwatch(F&& fn)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto result = fn();
    auto end = std::chrono::high_resolution_clock::now();
    //assert( result == expected );
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e9;
}

int main()
{
    //printf("okie, wrote %d numbers\n", compress());
    printf("operation took %lf s\n", stopwatch(compress));
}

