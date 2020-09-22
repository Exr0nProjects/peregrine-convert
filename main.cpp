#include <cstdio>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define isdigit(x) (x >= '0' && x <= '9')
#define isnumchar(x) ((x >= '0' && x <= '9') || x == '-' || x == '.')
#define tonum(x) (x-'0') // could be replaced with a hex conversion expression, for example

const size_t  IN_BUFFER_SIZE = 16*1024;
const size_t OUT_BUFFER_SIZE = 16;

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

void compress()
{
    FILE* wf = fopen("compressed.bin", "wb");
    //const size_t BUFFER_SIZE = 16*1024;
    int rfd = open("data.csv", O_RDONLY | O_NONBLOCK);

    if (~rfd) printf("reading file...\n");
    else { printf("Invalid input file\n"); return; }

    char  ibuf[ IN_BUFFER_SIZE*sizeof(char)];
    val_t obuf[OUT_BUFFER_SIZE*sizeof(val_t)];
    obuf[0] = 0;

    val_t *op = obuf; val_t neg = 1, dec = 0;
    while (size_t bytes_read = read(rfd, ibuf, IN_BUFFER_SIZE-1))
    {
        for (char *ip=ibuf; ip<ibuf+bytes_read; ++ip)
        {
            printf("    buf[%6d]: char '%d'    cur %10.2f     neg %2.0f dec %8.8f\n", ip-ibuf, *ip, neg, dec);
            if (!isnumchar(*ip))
            {
                if (dec) // was processing a number
                    if (++op > obuf+OUT_BUFFER_SIZE)
                        op = obuf /* TODO: flush to file */;
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
//void prep()
//{
//    int num_cnt = 0;
//    FILE* wptr = fopen("compressed.bin", "wb");
//    static const auto BUFFER_SIZE = 16*1024;
//    int fd = open("data.csv", O_RDONLY | O_NONBLOCK);
//    if(fd == -1) {
//        printf("fd == -1\n");
//    }
//    float tmp;
//    char buf[BUFFER_SIZE + 1];
//    while(size_t bytes_read = read(fd, buf, BUFFER_SIZE))
//    {
//        if(bytes_read == (size_t)-1) {
//            printf("bytes_read == (size_t)-1\n");
//        }
//        if (!bytes_read) break;
//        for(char *p = buf;;) {
//            char* bound = (char*) memchr(p, '\n', (buf + bytes_read) - p);
//            if (bound - p < 0) break; // Stop.
//            for (int i=0; i<5; ++i) {
//                tmp = scan(&p);
//                proc[num_cnt++ % DEBUG_BUF_SIZE] = tmp;
//                fwrite((void*)&tmp, sizeof(float), 1, wptr);
//            }
//            p = bound + 1;
//            putc('\n', wptr);
//        }
//    }
//}
//float read()
//{
//    int num_cnt = 0;
//    static const auto BUFFER_SIZE = 1024*1024;
//    int fd = open("data.csv", O_RDONLY | O_NONBLOCK);
//    if(fd == -1) {
//        printf("fd == -1\n");
//        return 1;
//    }
//    char buf[BUFFER_SIZE + 1];
//    uintmax_t lines = 0;
//    float tmp;
//    while(size_t bytes_read = read(fd, buf, BUFFER_SIZE))
//    {
//        if(bytes_read == (size_t)-1) {
//            printf("bytes_read == (size_t)-1\n");
//            return 1;
//        }
//        if (!bytes_read) break;
//        for(char *p = buf;;) {
//            char* bound = (char*) memchr(p, '\n', (buf + bytes_read) - p);
//            if (bound - p < 0) break; // Stop.
//            for (int i=0; i<5; ++i) {
//                if (proc[num_cnt++ % DEBUG_BUF_SIZE] != tmp)
//                    printf("printf %3d mismatech: %f vs %f\n", num_cnt-1, proc[(num_cnt-1) % DEBUG_BUF_SIZE], tmp);
//                tmp = *((float*)p);
//                p += sizeof(float);
//            }
//            p = bound + 1;
//            ++lines;
//        }
//    }
//    return tmp;
//}

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
    printf("okie\n");
    //prep();
    //scanf("%*c");
    //read();
}

