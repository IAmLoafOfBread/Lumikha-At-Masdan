#ifndef UTILS_H
#define UTILS_H



#include <stdint.h>

#if defined(RUN_DEBUG)
#include <stdio.h>
#define CHECK(val) printf("API CHECK -> %d   %s\n", val, #val);
#elif defined(RUN_PRODUCT)
#define CHECK(val) val
#endif

#define nullptr_VALUE -2147483640
#define nullptr_FLOAT -2147483640.0f
#define SIZE_OF(array) (sizeof(array) / sizeof(array[0]))

struct float2 {
	float u;
	float v;
};
struct float3 {
	float x;
	float y;
	float z;
};
struct float4 {
	float x;
	float y;
	float z;
	float w;
};

struct float4x4 {
	float4 vecs[4];
};



#endif
