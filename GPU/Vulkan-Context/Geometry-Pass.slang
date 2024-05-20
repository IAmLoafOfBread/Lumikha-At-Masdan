#define SHADER_INCLUDE
#include "../Shader-Utilities.h"



struct VSVertexInput {
    [[vk::location(0)]] float3 position;
    [[vk::location(1)]] float3 normal;
    [[vk::location(2)]] float3 tangent;
    [[vk::location(3)]] float2 uv;
};

struct VSInstanceInput {
    [[vk::location(4)]] float3 position;
    [[vk::location(5)]] float3 rotation;
};

struct VSOutput {
    float4 viewPosition : SV_POSITION;
    [[vk::location(0)]] float3 worldPosition;
    [[vk::location(1)]] float3 normal;
    [[vk::location(2)]] float3 tangent;
    [[vk::location(3)]] float2 uv;
    [[vk::location(4)]] nointerpolation uint textureIndex;
};

struct FSOutput {
    float4 position: SV_TARGET0;
    float4 albedo: SV_TARGET1;
    float4 normal: SV_TARGET2;
    float metalness: SV_TARGET3;
    float roughness: SV_TARGET4;
};

[[vk::binding(0 + LIGHTING_PASS_INPUT_COUNT, 0)]] Texture2D<float3> g_albedoTextures[];
[[vk::binding(1 + LIGHTING_PASS_INPUT_COUNT, 0)]] Texture2D<float3> g_normalTextures[];
[[vk::binding(2 + LIGHTING_PASS_INPUT_COUNT, 0)]] Texture2D<float> g_metalnessTextures[];
[[vk::binding(3 + LIGHTING_PASS_INPUT_COUNT, 0)]] Texture2D<float> g_roughnessTextures[];

[[vk::push_constant]] ConstantBuffer<View> g_view;

[shader("vertex")] VSOutput output_vertex(VSVertexInput in_vertex, VSInstanceInput in_instance) {
    VSOutput Output = { 0 };
    float3 Position = in_vertex.position;
    
    rotate_vector(Position, in_instance.rotation);
    Position += in_instance.position;
    Output.worldPosition = Position;
    Output.uv = in_vertex.uv;
    Output.uv *= -1;
    Output.uv += 1;
    Output.textureIndex = spirv_asm {
        OpCapability DrawParameters;
        result:$$uint = OpLoad builtin(DrawIndex:uint);
    };;

    return Output;
}

[shader("fragment")] FSOutput output_fragment(VSOutput in_vertex) {
    FSOutput lol;
    return lol;
}
