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

[[vk::binding(6, 0)]] Sampler2D<float> g_cascadedShadowMapsDB1[MAX_LIGHT_COUNT];
[[vk::binding(7, 0)]] Sampler2D<float> g_cascadedShadowMapsDB2[MAX_LIGHT_COUNT];
[[vk::binding(8, 0)]] Sampler2D<float> g_cascadedShadowMapsDB3[MAX_LIGHT_COUNT];

struct Constants {
    Light* lights;
    float3 position;
    uint lightCount;
};

[[vk::push_constant]] Constants g_constants;

float G1(float3 N, float3 X, float k) {
    float NX = dot(N, X);
    return NX / ((NX * (1 - k)) + k);
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
    float4 Output = float4(0, 0, 0, 1);

    float3 PointPosition = g_positionInput.SampleLevel(in_uv, 0).xyz;
    float3 PointNormal = normalize((g_normalInput.SampleLevel(in_uv, 0).xyz * 2) - 1);
    float3 Albedo = g_albedoInput.SampleLevel(in_uv, 0).xyz;
    float3 Specular = g_specularInput.SampleLevel(in_uv, 0).xyz;
    float A = g_roughnessInput.SampleLevel(in_uv, 0); A *= A;
    float R = A * A; A /= 2;
    float3 V = normalize(g_constants.position - PointPosition);
    float VN = dot(V, PointNormal);

    for (uint i = 0; i < g_constants.lightCount; i++) {
        Light CurrentLight = g_constants.lights[i];
        //if (CurrentLight.visible) {
            float3 LightPosition = CurrentLight.view.position;

            float3 L = normalize(LightPosition - PointPosition);
            float3 H = normalize(V + L);
            float PL = dot(PointPosition, L);
            float NL = dot(PointNormal, L);
            float NH = dot(PointNormal, H); NH *= NH;

            float D = R / (PI * pow((NH * (R - 1)) + 1, 2));
            float G = G1(PointNormal, V, A) * G1(PointNormal, L, A);
            float3 F0 = float3(0.2f);
            float3 Ks = F0 + ((float(1.0f) - F0) * pow(1 - dot(V, H), 5));

            float3 Fs = (D * G * Ks) / (4 * VN * NL);
            float3 Fd = (Albedo / PI) * NL;

            float3 Fr = ((float3(1.0f) - Ks) * Fd) + Fs;

            float VR = dot(V, reflect(L, PointNormal));
            float3 S = max(VR, 0.0f) * Specular;

            float LightDistance = distance(LightPosition, PointPosition);
            LightDistance *= LightDistance;
            Output.xyz += Fr * (CurrentLight.colour / LightDistance) + S;
        //}
    }

    return Output;
}

