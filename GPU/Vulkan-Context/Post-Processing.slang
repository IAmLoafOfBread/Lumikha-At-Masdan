#define SHADER_INCLUDE
#include "../Shader-Utilities.hpp"



struct VSOutput {
    float4 position : SV_POSITION;
    [[vk_location(0)]] float2 uv;
};

[[vk::binding(0, 0)]] Sampler2D<float4> g_frameInput;
[[vk::binding(1, 0)]] Sampler2D<float4> g_positionInput;
[[vk::binding(2, 0)]] Sampler2D<float4> g_normalInput;
[[vk::binding(3, 0)]] Sampler2D<float> g_depthInput;

struct Constants {
    float* reflectionSamples;
    float* occlusionSamples;
};
[[vk::push_constant]] Constants g_constants;

float3 reflect(float2 in_uv) {
	float3 Output = g_frameInput.SampleLevel(in_uv, 0).xyz;
	float3 PointPosition = g_positionInput.SampleLevel(in_uv, 0).xyz;
	float3 PointNormal = (g_normalInput.SampleLevel(in_uv, 0).xyz * 2) - 1;

	uint ReflectionCount = 0;
	Light Reflections[REFLECTION_SAMPLE_COUNT] = { 0 };
	for (uint i = 0; i < REFLECTION_SAMPLE_COUNT; i++) {
		float2 Sample = in_uv;
		
		float2 Step = float2(g_constants.reflectionSamples[i*2], g_constants.reflectionSamples[(i*2)+1]);
		uint RefInd = 0;
		while (Sample.x <= 1.0f && Sample.x >= 0.0f && Sample.y <= 1.0f && Sample.y >= 0.0f) {
			Sample += Step;
			
			if (dot(g_positionInput.SampleLevel(Sample, 0).xyz - PointPosition, PointNormal) > 0) {
				Reflections[i].position = g_positionInput.SampleLevel(in_uv, 0).xyz;
				Reflections[i].colour = g_frameInput.SampleLevel(in_uv, 0).xyz * (1.0f - dot(PointNormal, (g_normalInput.SampleLevel(Sample, 0).xyz * 2) - 1));
				ReflectionCount++;
				break;
			}
		}
	}

    for (uint i = 0; i < ReflectionCount; i++) {
        Light CurrentLight = Reflections[i];
			
		float LightDistance = distance(CurrentLight.position, PointPosition);
		LightDistance *= LightDistance;
		Output.xyz += CurrentLight.colour / LightDistance;
    }
	return Output;
}

float occlude(float2 in_uv) {
	float Greats = OCCLUSION_SAMPLE_COUNT;
	float Depth = g_depthInput.SampleLevel(in_uv, 0);
	for(uint i = 0; i < OCCLUSION_SAMPLE_COUNT; i++) {
		float2 Sample = in_uv;
		Sample += float2(g_constants.occlusionSamples[i*2], g_constants.occlusionSamples[(i*2)+1]);
		Sample = saturate(Sample);
		
		float Comp = g_depthInput.SampleLevel(Sample, 0);
		if(Depth < Comp) {
			float Dist = distance(Depth, Comp);
			if(Dist > 0.00005f && Dist < 0.0005f) {
				Greats--;
			}
		}
	}
	
	return Greats / OCCLUSION_SAMPLE_COUNT;
}

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
    return float4(reflect(in_uv) * occlude(in_uv), 1.0f);
}

