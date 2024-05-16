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


#endif
