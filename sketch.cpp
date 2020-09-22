#include <cstdio>

//float conv(int n)
//{
//    float ret=0;
//    for (int i=0; i<sizeof(int)<<3; ++i)
//        if (n>>i&1) ret |= 1<<i;
//    return ret;
//}
int main()
{
    //float n = 0x7203913f;
    //float n = 0xe0dbe43e;
    char n[4] = {0x72, 0x03, 0x9a, 0x3f};
    char *p = n;
    printf("%f\n", *reinterpret_cast<float*>(p));
}

