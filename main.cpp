#include <cstdio>
#include <chrono>
#include <string>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define isdigit(x) (x >= '0' && x <= '9')
#define isnumchar(x) (isdigit(x) || x == '-' || x == '.')
#define tonum(x) (x-'0')    // should convert anything that isdigit() and isn't '-' or '.'
                            // could be replaced with a hex conversion expression, for example

const size_t  IN_BUFFER_SIZE = 256*1024;    // chars
//const size_t OUT_BUFFER_SIZE =  64*1024;    // floats
const size_t OUT_BUFFER_SIZE =  64*1024;    // floats

const size_t DEBUG_BUF_SIZE = 1e5;
float proc[DEBUG_BUF_SIZE];

typedef float val_t;

//size_t compress(const std::string &infile, const std::string &outfile)
//{
//    FILE* wf = fopen(outfile.c_str(), "wb");
//    int rfd = open(infile.c_str(), O_RDONLY | O_NONBLOCK);
//    size_t written = 0;
//
//    if (~rfd) printf("reading file...\n");
//    else { printf("Invalid input file\n"); return 0; }
//
//    char  ibuf[ IN_BUFFER_SIZE*sizeof(char)];
//    //val_t obuf[OUT_BUFFER_SIZE*sizeof(val_t)];
//    //obuf[0] = 0;
//
//    val_t tmp; val_t neg = 1, dec = 0;
//    while (size_t bytes_read = read(rfd, ibuf, IN_BUFFER_SIZE-1))
//    {
//        for (char *ip=ibuf; ip<ibuf+bytes_read; ++ip)
//        {
//            //printf("    buf[%6d]: char '%d'    cur %10.2f     neg %2.0f dec %8.8f\n", ip-ibuf, *ip, neg, dec);
//            if (!isnumchar(*ip))
//            {
//                if (dec) // was processing a number
//                    fwrite((void*)&tmp, sizeof(val_t), 1, wf);
//                tmp = 0, neg = 1, dec = 0;
//            }
//            else if (*ip == '-') neg = -1;
//            else if (*ip == '.') dec =  1;
//            else    // it's a digit
//            {
//                if (dec > 0) tmp += (dec /= 10) * tonum(*ip);
//                else        (tmp *= 10) += tonum(*ip), dec=-1;
//            }
//        }
//    }
//    return written;
//}

size_t compress2(const std::string &infile, const std::string &outfile)
{
    FILE* wf = fopen(outfile.c_str(), "wb");
    int rfd = open(infile.c_str(), O_RDONLY | O_NONBLOCK);
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
            if (isdigit(*ip))
            {
                if (dec > 0) *op += (dec /= 10) * tonum(*ip);
                else        (*op *= 10) += tonum(*ip), dec=-1;
            }
            else if (*ip == '-') neg = -1;
            else if (*ip == '.') dec =  1;
            else
            {
                if (dec) // was processing a number
                    if (++op > obuf+OUT_BUFFER_SIZE)
                    {
                        written += fwrite((void*)obuf, sizeof(val_t), OUT_BUFFER_SIZE, wf);
                        op = obuf;
                    }
                *op = 0, neg = 1, dec = 0;
            }
        }
    }
    return written + fwrite((void*)obuf, sizeof(val_t), op-obuf, wf);
}

void read(const std::string &infile, const std::string &outfile, const size_t num_cnt)
{
    //FILE* wf = fopen(outfile.c_str(), "wb");
    //int rfd = open(infile.c_str(), O_RDONLY);
    int rfd = open("compressed2.bin", O_RDONLY);
    if (~rfd) printf("reading file...\n");
    else { printf("Invalid input file\n"); return; }

    float ibuf[OUT_BUFFER_SIZE*sizeof(float)];
    //val_t obuf[OUT_BUFFER_SIZE*sizeof(val_t)];
    size_t bytes_read = read(rfd, ibuf, 64);
    printf("bytes_read %lu\n", bytes_read);

    //for (size_t i=0; i<num_cnt; ++i)
    //{
    //    size_t bytes_read = read(rfd, ibuf, OUT_BUFFER_SIZE*sizeof(float));
    //    printf("read %lu bytes\n", bytes_read);
    //    if (!bytes_read) break;
    //    for (int j=i; i<num_cnt && i-j<bytes_read; ++i)
    //        printf("%d %f\n", i-j, ibuf[i-j]);
    //}

    //size_t i=0;
    //while (true)
    //{
    //    size_t bytes_read
    //    for (int i=1; i<bytes_read; ++i)
    //        printf("%10f\n", ibuf[i]);
    //    printf("%f %d\n", ibuf[0], bytes_read);
    //    break;
    //}
    //while ()
    //{
    //    printf("bytes read %lu\n", bytes_read);
    //    //for (int j=0; j<bytes_read/sizeof(float); ++j)
    //        //printf("%x\n", ibuf[i++]);
    //}

    //while (size_t bytes_read = read(rfd, ibuf, IN_BUFFER_SIZE))
    //{
    //    for (float *ip=ibuf; ip<ibuf+bytes_read; ++ip)
    //    {
    //        printf("got %f\n", *ip);
    //    }
    //}
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
    std::string infile = "data3.csv";
    std::string outfile1 = "compressed.bin";
    std::string outfile2 = "compressed2.bin";
    std::string infile_cmp = "data3proc.csv";
    //Sec time1 = stopwatch([](){ return 2; });
    Sec time2 = stopwatch(std::bind(compress2, infile, outfile2));
    printf("time = %f\n", time2);
    //double sped = time1/time2 * 100;
    //printf("operations took %lfs and %lfs for a %lf%% speedup\n", time1, time2, sped-100);
    //read(outfile2, infile_cmp, compress2(infile, outfile2));
    //printf("okie, wrote %d numbers\n", compress());
}

