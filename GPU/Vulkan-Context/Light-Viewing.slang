#define SHADER_INCLUDE
#include "../Shader-Utilities.hpp"

struct Constants {
    float3* subFrusta;
    Light* lights;
    uint lightCount;
};
[[vk::push_constant]] Constants g_constants;

[shader("compute")] [numthreads(CASCADED_SHADOW_MAP_COUNT, MAX_WORKGROUP_SIZE, 1)] void view_light(const uint in_frustumIndex: SV_GroupThreadID, const uint2 in_lightID: SV_DispatchThreadID) {
/*    const uint LightIndex = in_lightID.y;
    if (LightIndex >= g_constants.lightCount) return;
    const uint SubFrustumIndex = in_frustumIndex * CORNER_COUNT;

    Light* Light = &g_constants.lights[LightIndex];
    const float3 Position = Light->view.position;
    const float3 Rotation = Light->view.rotation;

    float3 Corners[CORNER_COUNT] = { 0 };
    for (uint i = 0; i < CORNER_COUNT; i++) {
        Corners[i] = g_constants.subFrusta[SubFrustumIndex + i];
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

    if (Mins.x >= Maxes.x || Mins.y >= Maxes.y) {
        Light->visible = false;
        return;
    }

    const float2 Midpoint = float2((Maxes.xy + Mins) / 2);
    const float Width = distance(Maxes.x, Mins.x);
    const float Height = distance(Maxes.y, Mins.y);

    Light->view.projection = float4x4(
        float4( 2 / Width,        0,           0,      0),
        float4(     0,       2 / Height,       0,      0),
        float4(     0,            0,     1 / Maxes.z,  0),
        float4(-Midpoint.x,  Midpoint.y,       0,      1)
    );*/
}
