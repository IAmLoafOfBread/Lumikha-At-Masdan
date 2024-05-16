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

typedef struct float2 {
	float u;
	float v;
} float2;
typedef struct float3 {
	float x;
	float y;
	float z;
} float3;
typedef struct float4 {
	float x;
	float y;
	float z;
	float w;
} float4;

typedef struct float4x4 {
	float4 vecs[4];
} float4x4;



#endif
