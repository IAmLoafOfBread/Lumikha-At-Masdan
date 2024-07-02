#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include "../../Build-Info.h"



#if !defined(SHADER_INCLUDE)
#include "../Vector/Vector.h"

#elif defined(SHADER_INCLUDE)

#if defined(SYSTEM_MACOS)
using namespace metal;
#endif

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

#define MAX_LIGHT_COUNT 5
#define MAX_WORKGROUP_SIZE 64
#define CASCADED_SHADOW_MAP_COUNT 3

#define GRAPHICS_THREAD_COUNT (1 + CASCADED_SHADOW_MAP_COUNT)
#define SHADOW_MAP_EXTENT {2080, 2080, 1}
#define SHADOW_VIEW_SIZE 1000

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#define GEOMETRY_PASS_COLOUR_ATTACHMENT_FORMATS \
	VK_FORMAT_R32G32B32A32_SFLOAT,\
	VK_FORMAT_R32G32B32A32_SFLOAT,\
	VK_FORMAT_R32G32B32A32_SFLOAT,\
	VK_FORMAT_R32G32B32A32_SFLOAT,\
	VK_FORMAT_R32_SFLOAT,\
	VK_FORMAT_R32_SFLOAT

#define GEOMETRY_PASS_REQUIRED_TEXTURE_FORMATS \
	VK_FORMAT_R16G16B16_UNORM,\
	VK_FORMAT_R16G16B16_UNORM,\
	VK_FORMAT_R16G16B16_UNORM,\
	VK_FORMAT_R16_UNORM,\
	VK_FORMAT_R16_UNORM
#endif

#define GEOMETRY_PASS_COLOUR_ATTACHMENT_COUNT 6
#define GEOMETRY_PASS_REQUIRED_TEXTURE_COUNT 5

#define CORNER_COUNT 8

struct Instance {
	float3 position;
	float3 rotation;
};
struct View {
	Instance instance;
	float4x4 projection;
};
struct Light {
	View view;
	float3 glare;
	float3 colour;
	bool visible;
};
struct Vertex {
	float3 position;
	float3 normal;
	float2 uv;
};
struct Camera {
	float fov;
	float zNear;
	float zFar;
};



#endif
