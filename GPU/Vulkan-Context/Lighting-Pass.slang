#define SHADER_INCLUDE
#include "../Shader-Utilities.h"



#define SHADOW_MAP_ATTACHMENTS_OFFSET /**/ (LIGHTING_PASS_INPUT_COUNT + DEFERRED_RENDERING_REQUIRED_TEXTURE_COUNT)

[[vk::input_attachment_index(0)]] let g_positionInput: SubpassInput<float3>;
[[vk::input_attachment_index(1)]] let g_albedoInput: SubpassInput<float3>;
[[vk::input_attachment_index(2)]] let g_normalInput: SubpassInput<float3>;
[[vk::input_attachment_index(3)]] let g_metalnessInput: SubpassInput<float>;
[[vk::input_attachment_index(4)]] let g_roughnessInput: SubpassInput<float>;

[[vk::binding(0 + SHADOW_MAP_ATTACHMENTS_OFFSET, 0]] let g_cascadedShadowMapsDB1: Texture2D<float>[];
[[vk::binding(1 + SHADOW_MAP_ATTACHMENTS_OFFSET, 0]] let g_cascadedShadowMapsDB2: Texture2D<float>[];
[[vk::binding(2 + SHADOW_MAP_ATTACHMENTS_OFFSET, 0]] let g_cascadedShadowMapsDB3: Texture2D<float>[];

[[vk::binding(SHADOW_MAP_ATTACHMENTS_OFFSET + CASCADED_SHADOW_MAP_COUNT, 0)]] let g_lights: ConstantBuffer<Light[]>;

[[vk::push_constant]] let g_lightCount: uint;

[shader("vertex")] func output_vertex(void) -> float2 {

}

[shader("fragment")] func output_fragment(in_position: float2) -> float4 {

}

