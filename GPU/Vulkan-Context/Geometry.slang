#define SHADER_INCLUDE
#include "../Shader-Utilities.hpp"



struct FSInput {
    [[vk::location(0)]] float3 worldPosition;
    [[vk::location(2)]] float3 worldRotation;
    [[vk::location(1)]] float3 normal;
    [[vk::location(3)]] float2 uv;
    [[vk::location(4)]] nointerpolation uint textureIndex;
};

struct VSOutput {
    float4 viewPosition : SV_POSITION;
    FSInput fragmentShaderInput;
};

struct FSOutput {
    float4 position: SV_TARGET0;
    float4 normal: SV_TARGET1;
    float4 albedo: SV_TARGET2;
    float4 specular: SV_TARGET3;
    float metalness: SV_TARGET4;
    float roughness: SV_TARGET5;
};

[[vk::binding(0, 0)]] Sampler2D<float4> g_normalTextures[];
[[vk::binding(1, 0)]] Sampler2D<float4> g_albedoTextures[];
[[vk::binding(2, 0)]] Sampler2D<float4> g_specularTextures[];
[[vk::binding(3, 0)]] Sampler2D<float> g_metalnessTextures[];
[[vk::binding(4, 0)]] Sampler2D<float> g_roughnessTextures[];

[[vk::push_constant]] View g_view;

[shader("vertex")] VSOutput output_vertex(
    [[vk::location(0)]] float3 inv_position,
    [[vk::location(1)]] float3 inv_normal,
    [[vk::location(2)]] float2 inv_uv,
    [[vk::location(3)]] float3 ini_position,
    [[vk::location(4)]] float3 ini_rotation
) {
    VSOutput Output = { 0 };
    float3 Position = inv_position;
    
    rotate_vector(Position, ini_rotation);
    Position += ini_position;
    Output.fragmentShaderInput.worldPosition = Position;
    Output.fragmentShaderInput.worldRotation = ini_rotation;
    Output.fragmentShaderInput.normal = inv_normal;
    rotate_vector(Output.fragmentShaderInput.normal, ini_rotation);
    Output.fragmentShaderInput.uv = inv_uv;
    Output.fragmentShaderInput.uv.y *= -1.0f;
    Output.fragmentShaderInput.uv.y += 1.0f;
    Output.fragmentShaderInput.textureIndex = spirv_asm {
        OpCapability DrawParameters;
        result:$$uint = OpLoad builtin(DrawIndex:uint);
    };
    Position -= g_view.instance.position;
    rotate_vector(Position, g_view.instance.rotation);
    Output.viewPosition = mul(float4(Position, 1), g_view.projection);

    return Output;
}

[shader("fragment")] FSOutput output_fragment(FSInput in_vertex) {
    FSOutput Output = { 0 };
    const uint Index = in_vertex.textureIndex;
    const float2 UV = in_vertex.uv;

    Output.position = float4(in_vertex.worldPosition, 1);
    {
        float3 Normal = g_normalTextures[Index].SampleLevel(UV, 0).xyz;
        rotate_vector(Normal, in_vertex.worldRotation);
        Output.normal = float4(Normal, 1);
    }
    Output.albedo = float4(g_albedoTextures[Index].SampleLevel(UV, 0));
    Output.specular = float4(g_specularTextures[Index].SampleLevel(UV, 0));
    Output.metalness = g_metalnessTextures[Index].SampleLevel(UV, 0);
    Output.roughness = g_roughnessTextures[Index].SampleLevel(UV, 0);

    return Output;
}
