#define SHADER_INCLUDE
#include "../Shader-Utilities.hpp"

struct VSVertexInput {
    [[vk::location(0)]] float3 position;
    [[vk::location(1)]] float3 normal;
    [[vk::location(3)]] float2 uv;
};

struct VSInstanceInput {
    [[vk::location(4)]] float3 position;
    [[vk::location(5)]] float3 rotation;
};

struct VSOutput {
    float4 viewPosition : SV_POSITION;
    [[vk::location(0)]] float3 worldPosition;
    [[vk::location(2)]] float3 worldRotation;
    [[vk::location(1)]] float3 normal;
    [[vk::location(3)]] float2 uv;
    [[vk::location(4)]] nointerpolation uint textureIndex;
};

struct FSOutput {
    float4 position: SV_TARGET0;
    float4 normal: SV_TARGET1;
    float4 albedo: SV_TARGET2;
    float4 specular: SV_TARGET3;
    float metalness: SV_TARGET4;
    float roughness: SV_TARGET5;
};

[[vk::binding(0, 0)]] Sampler2D<float3> g_normalTextures[];
[[vk::binding(1, 0)]] Sampler2D<float3> g_albedoTextures[];
[[vk::binding(2, 0)]] Sampler2D<float3> g_specularTextures[];
[[vk::binding(3, 0)]] Sampler2D<float> g_metalnessTextures[];
[[vk::binding(4, 0)]] Sampler2D<float> g_roughnessTextures[];

[[vk::push_constant]] View g_view;

[shader("vertex")] VSOutput output_vertex(VSVertexInput in_vertex, VSInstanceInput in_instance) {
    VSOutput Output = { 0 };
    float3 Position = in_vertex.position;
    
    rotate_vector(Position, in_instance.rotation);
    Position += in_instance.position;
    Output.worldPosition = Position;
    Output.worldRotation = in_instance.rotation;
    Output.normal = in_vertex.normal;
    rotate_vector(Output.normal, in_instance.rotation);
    Output.uv = (in_vertex.uv * -1) + 1;
    Output.textureIndex = spirv_asm {
        OpCapability DrawParameters;
        result:$$uint = OpLoad builtin(DrawIndex:uint);
    };
    Position -= g_view.instance.position;
    rotate_vector(Position, g_view.instance.rotation);
    Output.viewPosition = mul(float4(Position, 1), g_view.projection);

    return Output;
}

[shader("fragment")] FSOutput output_fragment(VSOutput in_vertex) {
    FSOutput Output = { 0 };
    const uint Index = in_vertex.textureIndex;
    const float2 UV = in_vertex.uv;

    Output.position = float4(in_vertex.worldPosition, 1);
    {
        float3 Normal = g_normalTextures[Index].SampleLevel(UV, 0);
        rotate_vector(Normal, in_vertex.worldRotation);
        Output.normal = float4(Normal, 1);
    }
    Output.albedo = float4(g_albedoTextures[Index].SampleLevel(UV, 0), 1);
    Output.specular = float4(g_specularTextures[Index].SampleLevel(UV, 0), 1);
    Output.metalness = g_metalnessTextures[Index].SampleLevel(UV, 0);
    Output.roughness = g_roughnessTextures[Index].SampleLevel(UV, 0);

    return Output;
}
