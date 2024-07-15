#define SHADER_INCLUDE
#include "../Shader-Utilities.hpp"



[[vk::push_constant]] View g_view;

[shader("vertex")] float4 output_vertex(
    [[vk::location(0)]] float3 inv_position,
    [[vk::location(1)]] float3 ini_position,
    [[vk::location(2)]] float3 ini_rotation
) : SV_Postion {
    float3 Position = inv_position;

    rotate_vector(Position, ini_rotation);
    Position += ini_position;
    Position -= g_view.instance.position;
    rotate_vector(Position, -g_view.instance.rotation);
    
    float4 ViewPosition = mul(float4(Position.xyz, 1), g_view.projection);
    return ViewPosition;
}

