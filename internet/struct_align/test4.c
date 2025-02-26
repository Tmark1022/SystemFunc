/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 05 Nov 2024 02:38:16 PM CST
 @ File Name	: test4.c
 @ Description	: C Program to demonstrate the structure padding property
 ************************************************************************/
#include <stdio.h>

#pragma pack(4)

// Alignment requirements
// (typical 32 bit machine)

// char         1 byte
// short int    2 bytes
// int          4 bytes
// double       8 bytes

// structure A
typedef struct structa_tag {
    char c;
    short int s;
} structa_t;

// structure B
typedef struct structb_tag {
    short int s;
    char c;
    int i;
} structb_t;

// structure C
typedef struct structc_tag {
    char c;
    double d;
    int s;
} structc_t;

// structure D
typedef struct structd_tag {
    double d;
    int s;
    char c;
} structd_t;

int main()
{
    printf("sizeof(structa_t) = %lu\n", sizeof(structa_t));
    printf("sizeof(structb_t) = %lu\n", sizeof(structb_t));
    printf("sizeof(structc_t) = %lu\n", sizeof(structc_t));
    printf("sizeof(structd_t) = %lu\n", sizeof(structd_t));

    structc_t obj;
    printf("sizeof(obj) : %ld, obj's address : %ld, elem c : %ld, elem d : %ld, elem s : %ld\n", sizeof(obj), &obj, &obj.c, &obj.d, &obj.s);

    int bb, cc;
    long ret = (long)&cc - (long)&bb;
    printf("%ld, %ld, %ld\n", &bb, &cc, ret);




    return 0;
}
