#define SHADER_INCLUDE
#include "../Shader-Utilities.hpp"



struct Constants {
    float* subFrusta;
    Light* lights;
    uint lightCount;
};
[[vk::push_constant]] Constants g_constants;

[shader("compute")] [numthreads(CASCADED_SHADOW_MAP_COUNT, MAX_WORKGROUP_SIZE, 1)] void view_light(const uint2 in_mapID: SV_DISPATCHTHREADID) {
	uint LightIndex = in_mapID.y;
	uint CascadeIndex = in_mapID.x;
    if (LightIndex >= g_constants.lightCount || CascadeIndex >= CASCADED_SHADOW_MAP_COUNT) return;
    const uint SubFrustumIndex = CascadeIndex * CORNER_COUNT;

    Light IndexedLight = g_constants.lights[LightIndex];

    float3 Corners[CORNER_COUNT] = { 0 };
    for (uint i = 0; i < CORNER_COUNT; i++) {
		uint Index = SubFrustumIndex + (i * 3);
        Corners[i].x = g_constants.subFrusta[Index];
        Corners[i].y = g_constants.subFrusta[Index + 1];
        Corners[i].z = g_constants.subFrusta[Index + 2];
        Corners[i] -= IndexedLight.position;
        rotate_vector(Corners[i], -IndexedLight.rotation);
    }

    float3 Maxes = Corners[0];
    float2 Mins = Corners[0].xy;
    for (uint i = 0; i < CORNER_COUNT; i++) {
        Maxes.x = Corners[i].x > Maxes.x ? Corners[i].x : Maxes.x;
        Maxes.y = Corners[i].y > Maxes.y ? Corners[i].y : Maxes.y;
        Maxes.z = Corners[i].z > Maxes.z ? Corners[i].z : Maxes.z;
        Mins.x = Mins.x > Corners[i].x ? Corners[i].x : Mins.x;
        Mins.y = Mins.y > Corners[i].y ? Corners[i].y : Mins.y;
    }

    Maxes.x = Maxes.x > SHADOW_VIEW_SIZE ? SHADOW_VIEW_SIZE : Maxes.x;
    Maxes.y = Maxes.y > SHADOW_VIEW_SIZE ? SHADOW_VIEW_SIZE : Maxes.y;
    Mins.x = -SHADOW_VIEW_SIZE > Mins.x ? -SHADOW_VIEW_SIZE : Mins.x;
    Mins.y = -SHADOW_VIEW_SIZE > Mins.y ? -SHADOW_VIEW_SIZE : Mins.y;

    if (Mins.x >= Maxes.x || Mins.y >= Maxes.y || Maxes.z <= 0) {
        g_constants.lights[LightIndex].visible = false;
        return;
    }
    
    const float2 Midpoint = (Maxes.xy + Mins) / 2;
    const float2 Extent = Maxes.xy - Mins;
    float Fn = 0.1f - Maxes.z;
    g_constants.lights[LightIndex].projections[CascadeIndex] = float4x4(
        float4(2/Extent.x,      0,             0,         0),
        float4(     0,      2/Extent.y,        0,         0),
        float4(     0,            0,        1/Fn,         0),
        float4(       -Midpoint    ,      -Maxes.z/Fn,    1)
    );
    g_constants.lights[LightIndex].visible = true;
}
