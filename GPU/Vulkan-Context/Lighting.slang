#define SHADER_INCLUDE
#include "../Shader-Utilities.hpp"



#define PI 3.14159f



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
[[vk::binding(6, 0)]] Sampler2D<float> g_depthInput;

[[vk::binding(7, 0)]] Sampler2D<float> g_cascadedShadowMapsDB1[MAX_LIGHT_COUNT];
[[vk::binding(8, 0)]] Sampler2D<float> g_cascadedShadowMapsDB2[MAX_LIGHT_COUNT];
[[vk::binding(9, 0)]] Sampler2D<float> g_cascadedShadowMapsDB3[MAX_LIGHT_COUNT];

[[vk::binding(10, 0)]] Sampler2D<float4> g_previousOutput;

struct Constants {
    Light* lights;
    float* reflectionSamples;
    float* occlusionSamples;
    float3 position;
    uint lightCount;
};

[[vk::push_constant]] Constants g_constants;

float cascade(uint in_cascadeIndex, uint in_lightIndex, float2 in_uv) {
	if(in_cascadeIndex == 0) return g_cascadedShadowMapsDB1[in_lightIndex].SampleLevel(in_uv, 0);
	if(in_cascadeIndex == 1) return g_cascadedShadowMapsDB2[in_lightIndex].SampleLevel(in_uv, 0);
	if(in_cascadeIndex == 2) return g_cascadedShadowMapsDB3[in_lightIndex].SampleLevel(in_uv, 0);
	return 0.0f;
}

float G1(float3 N, float3 X, float k) {
    float NX = dot(N, X);
    return NX / ((NX * (1 - k)) + k);
}

float3 lighting(float3 in_position, float3 in_normal, float2 in_uv) {
	float3 Output = float3(0);

	uint ReflectionCount = 0;
	Light Reflections[REFLECTION_SAMPLE_COUNT] = { 0 };
	for (uint i = 0; i < REFLECTION_SAMPLE_COUNT; i++) {
		float2 Sample = in_uv;
		
		float2 Step = float2(g_constants.reflectionSamples[i*2], g_constants.reflectionSamples[(i*2)+1]);
		uint RefInd = 0;
		while (Sample.x <= 1.0f && Sample.x >= 0.0f && Sample.y <= 1.0f && Sample.y >= 0.0f) {
			Sample += Step;
			float3 Position = g_positionInput.SampleLevel(Sample, 0).xyz;
			
			if (dot(Position - in_position, in_normal) > 0) {
				float3 Normal = (g_normalInput.SampleLevel(Sample, 0).xyz * 2) - 1;
				Reflections[i].colour = g_previousOutput.SampleLevel(in_uv, 0).xyz * (1.0f - dot(in_normal, Normal));
				ReflectionCount++;
				break;
			}
		}
	}

    float3 Albedo = g_albedoInput.SampleLevel(in_uv, 0).xyz;
    float3 Specular = g_specularInput.SampleLevel(in_uv, 0).xyz;
    float A = g_roughnessInput.SampleLevel(in_uv, 0); A *= A;
    float R = A * A; A /= 2;
    float3 V = normalize(g_constants.position - in_position);
    float VN = dot(V, in_normal);

	uint LightCount = g_constants.lightCount;
    for (uint i = 0; i < LightCount + ReflectionCount; i++) {
        Light CurrentLight = i < LightCount ? g_constants.lights[i] : Reflections[i - LightCount];
        if (CurrentLight.visible) {
			float3 LightPosition = CurrentLight.position;
			
			bool Lit = true;
			{
				float3 Position = in_position - LightPosition;
				rotate_vector(Position, -CurrentLight.rotation);
				for(uint j = 0; j < CASCADED_SHADOW_MAP_COUNT; j++) {
					float4 Projected = mul(float4(Position, 1.0f), CurrentLight.projections[j]);
					Projected /= Projected.w;
					float Z = Projected.z;
					float2 UV = (Projected.xy * 0.5f) + 0.5f;
					if(Z < cascade(j, i, UV)) {
						Lit = false;
						break;
					}
				}
			}
			
			if (Lit) {
				float3 L = normalize(LightPosition - in_position);
				float3 H = normalize(V + L);
				float PL = dot(in_position, L);
				float NL = dot(in_normal, L);
				float NH = dot(in_normal, H); NH *= NH;

				float D = R / (PI * pow((NH * (R - 1)) + 1, 2));
				float G = G1(in_normal, V, A) * G1(in_normal, L, A);
				float3 F0 = float3(0.2f);
				float3 Ks = F0 + ((float(1.0f) - F0) * pow(1 - dot(V, H), 5));

				float3 Kd = (1 - Ks) * (1 - g_metalnessInput.SampleLevel(in_uv, 0));

				float3 Fs = (D * G * Ks) / (4 * VN * NL);
				float3 Fd = Kd * (Albedo / PI) * NL;

				float3 Fr = Fd + Fs;

				float LightDistance = distance(LightPosition, in_position);
				LightDistance *= LightDistance;
				Output.xyz += (Fr * CurrentLight.colour * Specular) / LightDistance;
            }
        }
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
    float3 Output = float3(0);

    float3 PointPosition = g_positionInput.SampleLevel(in_uv, 0).xyz;
    float3 PointNormal = normalize((g_normalInput.SampleLevel(in_uv, 0).xyz * 2) - 1);
    Output = lighting(PointPosition, PointNormal, in_uv);
	Output *= occlude(in_uv);
	
    return float4(Output, 1.0f);
}

