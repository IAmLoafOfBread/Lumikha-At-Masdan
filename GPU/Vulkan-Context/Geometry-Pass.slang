#define SHADER_INCLUDE
#include "../Shader-Utilities.h"



struct VertexOutput {
	var vertex: Vertex;
    var textureIndex: uint;
}

struct FragmentOutput {
    var position: float3;
    var albedo: float3;
    var normal: float3;
    var metalness: float;
    var roughness: float;
}

[[vk::binding(0 + LIGHTING_PASS_INPUT_COUNT, 0)]] let g_albedoTextures: Texture2D<float3>[];
[[vk::binding(1 + LIGHTING_PASS_INPUT_COUNT, 0)]] let g_normalTextures: Texture2D<float3>[];
[[vk::binding(2 + LIGHTING_PASS_INPUT_COUNT, 0)]] let g_metalnessTextures: Texture2D<float>[];
[[vk::binding(3 + LIGHTING_PASS_INPUT_COUNT, 0)]] let g_roughnessTextures: Texture2D<float>[];

[[vk::push_constant]] let g_view: ConstantBuffer<View>;

[shader("vertex")] func output_vertex(in_vertex: Vertex, in_instancePosition: float3, in_instanceRotation: float3, in_instanceTextureIndex: uint) -> VertexOutput {
    VertexOutput lol;
    return lol;
}

[shader("fragment")] func output_fragment(in_vertex: VertexOutput) -> FragmentOutput {
    FragmentOutput lol;
    return lol;
}
