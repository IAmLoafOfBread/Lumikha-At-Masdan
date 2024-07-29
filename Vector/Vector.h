#ifndef VECTOR_H
#define VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif



#include <stdint.h>
#include <math.h>

#define NULL_VALUE 2147483640
#define LENGTH_OF(array) (sizeof(array) / sizeof(array[0]))

enum Transform {
	POSITION,
	ROTATION
};

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
typedef struct float3x3 {
	float3 vecs[3];
} float3x3;
typedef struct float4x4 {
	float4 vecs[4];
} float4x4;

void invert_matrix(float4x4* in_matrix);
void transform_vector3(float3* in_vector, const float3x3* in_matrix);
void transform_vector4(float4* in_vector, const float4x4* in_matrix);
void rotate_vector(float3* in_vector, const float3 in_rotation);



#ifdef __cplusplus
}
#endif

#endif
