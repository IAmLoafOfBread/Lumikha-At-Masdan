#define SHADER_INCLUDE
#include "../Shader-Utilities.hpp"

struct VSOutput {
    float4 position : SV_Position;
    float2 uv;
};

[[vk::binding(0, 0)]] Sampler2D<float4> g_positionInput;
[[vk::binding(1, 0)]] Sampler2D<float4> g_normalInput;
[[vk::binding(2, 0)]] Sampler2D<float4> g_albedoInput;
[[vk::binding(3, 0)]] Sampler2D<float4> g_specularInput;
[[vk::binding(4, 0)]] Sampler2D<float> g_metalnessInput;
[[vk::binding(5, 0)]] Sampler2D<float> g_roughnessInput;

[[vk::binding(6, 0)]] Sampler2D<float> g_cascadedShadowMapsDB1[];
[[vk::binding(7, 0)]] Sampler2D<float> g_cascadedShadowMapsDB2[];
[[vk::binding(8, 0)]] Sampler2D<float> g_cascadedShadowMapsDB3[];

struct Constants {
    float4 positions[4];
    float2 uvs[4];
    Light* lights;
    uint lightCount;
};

[[vk::push_constant]] Constants g_constants;

[shader("vertex")] VSOutput output_vertex(uint in_index: SV_VertexID) {
    VSOutput Output = { 0 };
    
    Output.position = g_constants.positions[in_index];
    Output.uv = g_constants.uvs[in_index];
    
    return Output;
}

[shader("fragment")] float4 output_fragment(float2 in_position) {
    float4 yoyo;
    return yoyo;
}

