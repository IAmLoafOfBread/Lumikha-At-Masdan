#define SHADER_INCLUDE
#include "../Shader-Utilities.hpp"

struct VSVertexInput {
    [[vk::location(0)]] float3 position;
};

struct VSInstanceInput {
    [[vk::location(1)]] float3 position;
    [[vk::location(2)]] float3 rotation;
};

struct VSOutput {
    float4 viewPosition : SV_Position;
};

[[vk::push_constant]] View g_view;

[shader("vertex")] VSOutput output_vertex(VSVertexInput in_vertex, VSInstanceInput in_instance) {
    VSOutput Output = { 0 };
    float3 Position = in_vertex.position;

    rotate_vector(Position, in_instance.rotation);
    Position += in_instance.position;
    Position -= g_view.instance.position;
    rotate_vector(Position, -g_view.instance.rotation);
    Output.viewPosition = mul(float4(Position.xyz, 1), g_view.projection);

    return Output;
}

