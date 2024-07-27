#define SHADER_INCLUDE
#include "../Shader-Utilities.hpp"

struct Constants {
    float* subFrusta;
    Light* lights;
    uint lightCount;
};
[[vk::push_constant]] Constants g_constants;

[shader("compute")] [numthreads(CASCADED_SHADOW_MAP_COUNT, MAX_WORKGROUP_SIZE, 1)] void view_light(const uint2 in_mapID: SV_DISPATCHTHREADID) {
	uint LightID = in_mapID.y;
    if (LightID >= g_constants.lightCount) return;
    const uint SubFrustumIndex = in_mapID.x * CORNER_COUNT;

    Light IndexedLight = g_constants.lights[LightID];
    const float3 Position = IndexedLight.view.position;
    const float3 Rotation = IndexedLight.view.rotation;

    float3 Corners[CORNER_COUNT] = { 0 };
    for (uint i = 0; i < CORNER_COUNT; i++) {
        Corners[i] = (float3)(g_constants.subFrusta[(SubFrustumIndex * CORNER_COUNT) + (i * 3)]);
        Corners[i] -= Position;
        rotate_vector(Corners[i], -Rotation);
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
        g_constants.lights[LightID].visible = false;
        return;
    }
    
    IndexedLight.visible = true;

    const float2 Midpoint = float2((Maxes.xy + Mins) / 2);
    const float Width = distance(Maxes.x, Mins.x);
    const float Height = distance(Maxes.y, Mins.y);

    IndexedLight.view.projection = float4x4(
        float4( 2 / Width,        0,           0,      0),
        float4(     0,       2 / Height,       0,      0),
        float4(     0,            0,     1 / Maxes.z,  0),
        float4(       -Midpoint    ,           0,      1)
    );
    g_constants.lights[LightID] = IndexedLight;
}
