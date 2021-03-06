//
//  fileread.cpp
//  Jacobian
//
//  straight up yoinked from David Freifeld
//
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <ctime>
#include <chrono>
#include <iostream>
#include <cmath>
#include <stdint.h>

// No safety checks whatsoever. Use at your own risk.
// Assumes solely numeric input. No NaN, no inf.
// float strtof_but_bad(char* p)
// {
//     float flt;
//     for (int i = 0;
// }

// Courtesy of github.com/exr0n
typedef float val_t;
inline float scan(char **p)
{
    float n;
    int neg = 1;
    while (!isdigit(**p) && **p != '-' && **p != '.') ++*p;
    if (**p == '-') neg = -1, ++*p;
    for (n=0; isdigit(**p); ++*p)
        (n *= 10) += (**p-'0');
    if (*(*p)++ != '.') return n*neg;
    for (float d=1; isdigit(**p); ++*p)
        n += (d /= 10) * (**p-'0');
    return n*neg;
}

float current()
{
    static const auto BUFFER_SIZE = 16*1024;
    int fd = open("data.csv", O_RDONLY & O_NONBLOCK);
    if(fd == -1) {
        printf("fd == -1\n");
        return 1;
    }

    char buf[BUFFER_SIZE + 1];
    uintmax_t lines = 0;
    float tmp;
    while(size_t bytes_read = read(fd, buf, BUFFER_SIZE))
    {
        if(bytes_read == (size_t)-1) {
            printf("bytes_read == (size_t)-1\n");
            return 1;
        }
        if (!bytes_read) break;
        for(char *p = buf;;) {
            char* bound = (char*) memchr(p, '\n', (buf + bytes_read) - p);
            if (bound - p < 0) break; // Stop.
            for (int i=0; i<5; ++i) tmp = scan(&p);
            p = bound + 1;
            ++lines;
        }
    }
    return tmp;
}

void prep()
{
    FILE* wptr = fopen("compressed.bin", "wb");
    static const auto BUFFER_SIZE = 16*1024;
    int fd = open("data.csv", O_RDONLY | O_NONBLOCK);
    if(fd == -1) {
        printf("fd == -1\n");
    }
    float tmp;
    char buf[BUFFER_SIZE + 1];
    while(size_t bytes_read = read(fd, buf, BUFFER_SIZE))
    {
        if(bytes_read == (size_t)-1) {
            printf("bytes_read == (size_t)-1\n");
        }
        if (!bytes_read) break;
        for(char *p = buf;;) {
            char* bound = (char*) memchr(p, '\n', (buf + bytes_read) - p);
            if (bound - p < 0) break; // Stop.
            for (int i=0; i<5; ++i) {
                tmp = scan(&p);
                fwrite((void*)&tmp, sizeof(float), 1, wptr);
            }
            p = bound + 1;
            putc('\n', wptr);
        }
    }
}

float newer()
{
    static const auto BUFFER_SIZE = 1024*1024;
    int fd = open("compressed.bin", O_RDONLY | O_NONBLOCK);
    if(fd == -1) {
        printf("fd == -1\n");
        return 1;
    }
    char buf[BUFFER_SIZE + 1];
    uintmax_t lines = 0;
    float tmp;
    while(size_t bytes_read = read(fd, buf, BUFFER_SIZE))
    {
        if(bytes_read == (size_t)-1) {
            printf("bytes_read == (size_t)-1\n");
            return 1;
        }
        if (!bytes_read) break;
        for(char *p = buf;;) {
            char* bound = (char*) memchr(p, '\n', (buf + bytes_read) - p);
            if (bound - p < 0) break; // Stop.
            for (int i=0; i<5; ++i) {
                tmp = *((float*)p);
                printf("%f\n", tmp);
                p += sizeof(float);
            }
            p = bound + 1;
            ++lines;
        }
    }
    return tmp;
}

int main () {
    auto start = std::chrono::high_resolution_clock::now();
    prep();
    auto prep_end = std::chrono::high_resolution_clock::now();
    float tmp = newer();
    auto end = std::chrono::high_resolution_clock::now();
    double time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - prep_end + prep_end-start).count() / pow(10,9);
    double runtime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - prep_end).count() / pow(10,9);

    auto f_start = std::chrono::high_resolution_clock::now();
    float ftmp = current();
    auto f_end = std::chrono::high_resolution_clock::now();
    // if (flines == lines) std::cout << "Linecount is valid!" << "\n";
    // else std::cout << "WARN: INVALID LINECOUNT " << lines << " vs. " << flines << "\n";
    if (ftmp == tmp) std::cout << "Final value read is valid!" << "\n";
    else std::cout << "WARN: INVALID LAST READ VAL " << tmp << " vs. " << ftmp << "\n";
    double ftime = std::chrono::duration_cast<std::chrono::nanoseconds>(f_end - f_start).count() / pow(10,9);
    std::cout << ftime << " " << time << " " << runtime << " (" << ftime/time * 100 << "% speedup overall, " << ftime/runtime * 100 << "% speedup runtime)\n";
}

