#ifndef TEST_STRUCT_H

#define TEST_STRUCT_H
#include <arpa/inet.h>

#define MAX_BUF_SIZE	1024

#define TYPE_ECHO 1 
#define TYPE_CALL 2

typedef struct {
	uint8_t type;
	uint32_t size;
} Proto;


#endif
