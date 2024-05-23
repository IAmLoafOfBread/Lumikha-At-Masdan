#define SHADER_INCLUDE
#include "../Shader-Utilities.h"

struct VSVertexInput {
    [[vk::location(0)]] float3 position;
};

struct VSInstanceInput {
    [[vk::location(1)]] float3 position;
    [[vk::location(2)]] float3 rotation;
};

struct VSOutput {
    float4 viewPosition : SV_POSITION;
};

struct Constants {
    View view;
    float3 offset;
};

[[vk::push_constant]] Constants g_constants;

[shader("vertex")] VSOutput output_vertex(VSVertexInput in_vertex, VSInstanceInput in_instance) {
    VSOutput Output = { 0 };
    View View = g_constants.view;
    float3 Position = in_vertex.position;

    rotate_vector(Position, in_instance.rotation);
    Position += in_instance.position;
    Position -= View.position;
    rotate_vector(Position, -View.rotation);
    Output.viewPosition = mul(float4(Position.xyz, 1), View.projection);

    return Output;
}

