#define SHADER_INCLUDE
#include "../Shader-Utilities.h"



#define SHADOW_MAP_ATTACHMENTS_OFFSET /**/(LIGHTING_PASS_INPUT_COUNT + DEFERRED_RENDERING_REQUIRED_TEXTURE_COUNT)

[[vk::binding(0, 0)]] Texture2D<float4> g_positionInput;
[[vk::binding(1, 0)]] Texture2D<float4> g_albedoInput;
[[vk::binding(2, 0)]] Texture2D<float4> g_normalInput;
[[vk::binding(3, 0)]] Texture2D<float> g_metalnessInput;
[[vk::binding(4, 0)]] Texture2D<float> g_roughnessInput;

[[vk::binding(0 + SHADOW_MAP_ATTACHMENTS_OFFSET, 0)]] Texture2D<float> g_cascadedShadowMapsDB1[MAX_LIGHT_COUNT];
[[vk::binding(1 + SHADOW_MAP_ATTACHMENTS_OFFSET, 0)]] Texture2D<float> g_cascadedShadowMapsDB2[MAX_LIGHT_COUNT];
[[vk::binding(2 + SHADOW_MAP_ATTACHMENTS_OFFSET, 0)]] Texture2D<float> g_cascadedShadowMapsDB3[MAX_LIGHT_COUNT];

[[vk::binding(SHADOW_MAP_ATTACHMENTS_OFFSET + CASCADED_SHADOW_MAP_COUNT, 0)]] ConstantBuffer<Light[]> g_lights;

[[vk::push_constant]] uint g_lightCount;

[shader("vertex")] float2 output_vertex(void) {
    float2 Haha;
    return Haha;
}

[shader("fragment")] float4 output_fragment(float2 in_position) {
    float4 yoyo;
    return yoyo;
}

