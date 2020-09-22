#include <cstdio>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define isdigit(x) (x >= '0' && x <= '9')


const size_t DEBUG_BUF_SIZE = 1e5;
float proc[DEBUG_BUF_SIZE];

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
//void prep()
//{
//    int num_cnt = 0;
//    FILE* wptr = fopen("compressed.bin", "wb");
//    static const auto BUFFER_SIZE = 16*1024;
//    int fd = open("data.csv", O_RDONLY | O_NONBLOCK);
//    if (fd == -1) {
//        printf("fd == -1\n");
//    }
//    val_t tmp;
//    char buf[BUFFER_SIZE + 1];
//    while (size_t bytes_read = read(fd, buf, BUFFER_SIZE))
//    {
//        if (bytes_read == (size_t)-1) {
//            printf("bytes_read == (size_t)-1\n");
//        }
//        if (!bytes_read) break;
//        for (char *p = buf;;) {
//            char* bound = (char*) memchr(p, '\n', (buf + bytes_read) - p);
//            if (bound - p < 0) break; // Stop.
//            for (int i=0; i<5; ++i) {
//                tmp = scan(&p);
//                proc[num_cnt++ % DEBUG_BUF_SIZE] = tmp;
//                fwrite((void*)&tmp, sizeof(val_t), 1, wptr);
//            }
//            p = bound + 1;
//        }
//    }
//}
void prep()
{
    int num_cnt = 0;
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
                proc[num_cnt++ % DEBUG_BUF_SIZE] = tmp;
                fwrite((void*)&tmp, sizeof(float), 1, wptr);
            }
            p = bound + 1;
            putc('\n', wptr);
        }
    }
}
float read()
{
    int num_cnt = 0;
    static const auto BUFFER_SIZE = 1024*1024;
    int fd = open("data.csv", O_RDONLY | O_NONBLOCK);
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
                if (proc[num_cnt++ % DEBUG_BUF_SIZE] != tmp)
                    printf("printf %3d mismatech: %f vs %f\n", num_cnt-1, proc[(num_cnt-1) % DEBUG_BUF_SIZE], tmp);
                tmp = *((float*)p);
                p += sizeof(float);
            }
            p = bound + 1;
            ++lines;
        }
    }
    return tmp;
}

//void read()
//{
//    int num_cnt = 0;
//    static const auto BUFFER_SIZE = 16*1024;
//    int fd = open("compressed.bin", O_RDONLY | O_NONBLOCK);
//    if (fd == -1) {
//        printf("fd == -1\n");
//    }
//    val_t tmp;
//    char buf[BUFFER_SIZE + 1];
//    while (size_t bytes_read = read(fd, buf, BUFFER_SIZE))
//    {
//        if (bytes_read == (size_t)-1) {
//            printf("bytes_read == (size_t)-1\n");
//        }
//        //if (!bytes_read) break;
//        for (char *p = buf; p < buf+BUFFER_SIZE;) {
//            //char* bound = (char*) memchr(p, '\n', (buf + bytes_read) - p);
//            //if (bound - p < 0) break; // Stop.
//            //for (int i=0; i<5; ++i) {
//            tmp = *(float*)p;
//            if (proc[num_cnt++ % DEBUG_BUF_SIZE] != tmp)
//                printf("printf %3d mismatech: %f vs %f\n", num_cnt-1, proc[(num_cnt-1) % DEBUG_BUF_SIZE], tmp);
//
//            p+=sizeof(float);
//            //}
//            //p = bound + 1;
//        }
//    }
//}

int main()
{
    prep();
    //scanf("%*c");
    read();
}

