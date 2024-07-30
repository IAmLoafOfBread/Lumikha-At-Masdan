#define SHADER_INCLUDE
#include "../Shader-Utilities.hpp"



struct Constants {
	float3 position;
	float3 rotation;
	float4x4 projection;
};
[[vk::push_constant]] Constants g_constants;

[shader("vertex")] float4 output_vertex(
    [[vk::location(0)]] float3 inv_position,
    [[vk::location(1)]] float3 ini_position,
    [[vk::location(2)]] float3 ini_rotation,
) : SV_POSITION {
    float3 Position = inv_position;
    rotate_vector(Position, ini_rotation);
    Position += ini_position;
    Position -= g_constants.position;
    rotate_vector(Position, -g_constants.rotation);
    
    return mul(float4(Position, 1.0f), g_constants.projection);
}

