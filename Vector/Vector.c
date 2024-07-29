#include "Vector.h"

void invert_matrix(float4x4* in_matrix) {
	float m[16];
	for(uint32_t i = 0; i < 4; i++) {
		m[i*4] = in_matrix->vecs[i].x;
		m[(i*4)+1] = in_matrix->vecs[i].y;
		m[(i*4)+2] = in_matrix->vecs[i].z;
		m[(i*4)+3] = in_matrix->vecs[i].w;
	}
	
	float inv[16], det;
	int i;
	
	inv[0] = m[5]  * m[10] * m[15] -
	m[5]  * m[11] * m[14] -
	m[9]  * m[6]  * m[15] +
	m[9]  * m[7]  * m[14] +
	m[13] * m[6]  * m[11] -
	m[13] * m[7]  * m[10];
	
	inv[4] = -m[4]  * m[10] * m[15] +
	m[4]  * m[11] * m[14] +
	m[8]  * m[6]  * m[15] -
	m[8]  * m[7]  * m[14] -
	m[12] * m[6]  * m[11] +
	m[12] * m[7]  * m[10];
	
	inv[8] = m[4]  * m[9] * m[15] -
	m[4]  * m[11] * m[13] -
	m[8]  * m[5] * m[15] +
	m[8]  * m[7] * m[13] +
	m[12] * m[5] * m[11] -
	m[12] * m[7] * m[9];
	
	inv[12] = -m[4]  * m[9] * m[14] +
	m[4]  * m[10] * m[13] +
	m[8]  * m[5] * m[14] -
	m[8]  * m[6] * m[13] -
	m[12] * m[5] * m[10] +
	m[12] * m[6] * m[9];
	
	inv[1] = -m[1]  * m[10] * m[15] +
	m[1]  * m[11] * m[14] +
	m[9]  * m[2] * m[15] -
	m[9]  * m[3] * m[14] -
	m[13] * m[2] * m[11] +
	m[13] * m[3] * m[10];
	
	inv[5] = m[0]  * m[10] * m[15] -
	m[0]  * m[11] * m[14] -
	m[8]  * m[2] * m[15] +
	m[8]  * m[3] * m[14] +
	m[12] * m[2] * m[11] -
	m[12] * m[3] * m[10];
	
	inv[9] = -m[0]  * m[9] * m[15] +
	m[0]  * m[11] * m[13] +
	m[8]  * m[1] * m[15] -
	m[8]  * m[3] * m[13] -
	m[12] * m[1] * m[11] +
	m[12] * m[3] * m[9];
	
	inv[13] = m[0]  * m[9] * m[14] -
	m[0]  * m[10] * m[13] -
	m[8]  * m[1] * m[14] +
	m[8]  * m[2] * m[13] +
	m[12] * m[1] * m[10] -
	m[12] * m[2] * m[9];
	
	inv[2] = m[1]  * m[6] * m[15] -
	m[1]  * m[7] * m[14] -
	m[5]  * m[2] * m[15] +
	m[5]  * m[3] * m[14] +
	m[13] * m[2] * m[7] -
	m[13] * m[3] * m[6];
	
	inv[6] = -m[0]  * m[6] * m[15] +
	m[0]  * m[7] * m[14] +
	m[4]  * m[2] * m[15] -
	m[4]  * m[3] * m[14] -
	m[12] * m[2] * m[7] +
	m[12] * m[3] * m[6];
	
	inv[10] = m[0]  * m[5] * m[15] -
	m[0]  * m[7] * m[13] -
	m[4]  * m[1] * m[15] +
	m[4]  * m[3] * m[13] +
	m[12] * m[1] * m[7] -
	m[12] * m[3] * m[5];
	
	inv[14] = -m[0]  * m[5] * m[14] +
	m[0]  * m[6] * m[13] +
	m[4]  * m[1] * m[14] -
	m[4]  * m[2] * m[13] -
	m[12] * m[1] * m[6] +
	m[12] * m[2] * m[5];
	
	inv[3] = -m[1] * m[6] * m[11] +
	m[1] * m[7] * m[10] +
	m[5] * m[2] * m[11] -
	m[5] * m[3] * m[10] -
	m[9] * m[2] * m[7] +
	m[9] * m[3] * m[6];
	
	inv[7] = m[0] * m[6] * m[11] -
	m[0] * m[7] * m[10] -
	m[4] * m[2] * m[11] +
	m[4] * m[3] * m[10] +
	m[8] * m[2] * m[7] -
	m[8] * m[3] * m[6];
	
	inv[11] = -m[0] * m[5] * m[11] +
	m[0] * m[7] * m[9] +
	m[4] * m[1] * m[11] -
	m[4] * m[3] * m[9] -
	m[8] * m[1] * m[7] +
	m[8] * m[3] * m[5];
	
	inv[15] = m[0] * m[5] * m[10] -
	m[0] * m[6] * m[9] -
	m[4] * m[1] * m[10] +
	m[4] * m[2] * m[9] +
	m[8] * m[1] * m[6] -
	m[8] * m[2] * m[5];
	
	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
	
	det = 1.0f / det;
	
	for (i = 0; i < 4; i++) {
		in_matrix->vecs[i].x = inv[i*4] * det;
		in_matrix->vecs[i].y = inv[(i*4)+1] * det;
		in_matrix->vecs[i].z = inv[(i*4)+2] * det;
		in_matrix->vecs[i].w = inv[(i*4)+3] * det;
	}
}

void transform_vector3(float3* in_vector, const float3x3* in_matrix) {
	float* InVector = (float*)in_vector;
	float* InMatrix = (float*)in_matrix;
	float3 Output = { 0 };
	float* OutVector = (float*)&Output;
	for(uint32_t i = 0; i < 3; i++) {
		for(uint32_t j = 0; j < 3; j++) {
			OutVector[i] += InVector[j] * InMatrix[(j * 3) + i];
		}
	}
	in_vector->x = OutVector[0];
	in_vector->y = OutVector[1];
	in_vector->z = OutVector[2];
}

void transform_vector4(float4* in_vector, const float4x4* in_matrix) {
	float* InVector = (float*)in_vector;
	float* InMatrix = (float*)in_matrix;
	float4 Output = { 0 };
	float* OutVector = (float*)&Output;
	for(uint32_t i = 0; i < 4; i++) {
		for(uint32_t j = 0; j < 4; j++) {
			OutVector[i] += InVector[j] * InMatrix[(j * 4) + i];
		}
	}
	in_vector->x = OutVector[0];
	in_vector->y = OutVector[1];
	in_vector->z = OutVector[2];
	in_vector->w = OutVector[3];
}

void rotate_vector(float3* in_vector, const float3 in_rotation) {
	const float CosX = cosf(in_rotation.x);
	const float CosY = cosf(in_rotation.y);
	const float CosZ = cosf(in_rotation.z);
	const float SinX = sinf(in_rotation.x);
	const float SinY = sinf(in_rotation.y);
	const float SinZ = sinf(in_rotation.z);
	const float SinX_SinY = SinX * SinY;
	const float CosX_SinY = CosX * -SinY;
	const float3x3 Matrix = {
		CosY * CosZ, CosY * -SinZ, SinY,
		(SinX_SinY * CosZ) + (CosX * SinZ), (SinX_SinY * -SinZ) + (CosX * CosZ), -SinX * CosY,
		(CosX_SinY * CosZ) + (SinX * SinZ), (CosX_SinY * -SinZ) + (SinX * CosZ), CosX * CosY
	};
	transform_vector3(in_vector, &Matrix);
}
