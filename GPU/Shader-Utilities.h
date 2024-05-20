#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H



#include "../../Build-Info.h"

#if !defined(SHADER_INCLUDE)

#include "../Utilities.h"
#define TYPEDEF(name) name

#elif defined(SHADER_INCLUDE)

#if defined(SYSTEM_MACOS)
using namespace metal;
#endif

#define typedef
#define TYPEDEF(name)
#define uint32_t uint

void rotate_vector(inout float3 in_vector, float3 in_rotation) {
    const float CosX = cos(in_rotation.x);
    const float CosY = cos(in_rotation.y);
    const float CosZ = cos(in_rotation.z);
    const float SinX = sin(in_rotation.x);
    const float SinY = sin(in_rotation.y);
    const float SinZ = sin(in_rotation.z);
    const float SinX_SinY = SinX * SinY;
    const float CosX_SinY = CosX * -SinY;
    const float3x3 Matrix = float3x3(
        float3(CosY * CosZ, CosY * -SinZ, SinY),
        float3((SinX_SinY * CosZ) + (CosX * SinZ), (SinX_SinY * -SinZ) + (CosX * CosZ), -SinX * CosY),
        float3((CosX_SinY * CosZ) + (SinX * SinZ), (CosX_SinY * -SinZ) + (SinZ * CosZ), CosX * CosY)
    );
    in_vector = mul(in_vector, Matrix);
}

#endif

typedef struct Vertex {
	float3 position;
	float3 normal;
	float2 uv;
} TYPEDEF(Vertex);
typedef struct Light {
	float3 position;
	float3 rotation;
	float3 glare;
	float3 colour;
} TYPEDEF(Light);
typedef struct Instance {
	float3 position;
	float3 rotation;
	uint32_t textureIndex;
} TYPEDEF(Instance);
typedef struct View {
	float3 position;
	float3 rotation;
	float4x4 projection;
} TYPEDEF(View);
typedef struct Camera {
	float3 position;
	float3 rotation;
	float fov;
	float zNear;
	float zFar;
} TYPEDEF(Camera);



#endif
