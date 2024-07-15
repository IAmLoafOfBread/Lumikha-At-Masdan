#define SHADER_INCLUDE
#include "../Shader-Utilities.hpp"



struct VSOutput {
    float4 position : SV_POSITION;
    [[vk_location(0)]] float2 uv;
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
    Light* lights;
    uint lightCount;
};

[[vk::push_constant]] Constants g_constants;

[shader("vertex")] VSOutput output_vertex(uint in_index: SV_VERTEXID) {
    VSOutput Output[] = {
        float4(-1.0f, 1.0f, 0.0f, 1.0f), float2(0.0f, 1.0f),
        float4(-1.0f, -1.0f, 0.0f, 1.0f), float2(0.0f, 0.0f),
        float4(1.0f, 1.0f, 0.0f, 1.0f), float2(1.0f, 1.0f),
        float4(1.0f, -1.0f, 0.0f, 1.0f), float2(1.0f, 0.0f)
    };
    return Output[in_index];
}

[shader("fragment")] float4 output_fragment(
    [[vk::location(0)]] float2 in_uv
) : SV_TARGET0 {
    float4 Output = float4(0, 0, 0, 1);
    Output = g_albedoInput.SampleLevel(in_uv, 0);
    return Output;
}

