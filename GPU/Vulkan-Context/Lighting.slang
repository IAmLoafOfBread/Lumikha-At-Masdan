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
    float3 position;
    uint lightCount;
    Light* lights;
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
    float3 PointNormal = g_normalInput.SampleLevel(in_uv, 0).xyz;
    float3 Albedo = g_albedoInput.SampleLevel(in_uv, 0).xyz;
    float3 Specular = g_specularInput.SampleLevel(in_uv, 0).xyz;
    float A = pow(g_roughnessInput.SampleLevel(in_uv, 0), 2);
    float R = pow(A, 2);
    A /= 2;
    float3 CameraPosition = g_constants.position;
    float3 ViewNormal = dot(CameraPosition, PointNormal);

/*    for (uint i = 0; i < g_constants.lightCount; i++) {
        Light CurrentLight = g_constants.lights[i];
        if (CurrentLight.visible) {
            float3 LightPosition = CurrentLight.view.instance.position;
            float3 PL_Dot = dot(PointPosition, LightPosition);
            float3 Half = (CameraPosition + LightPosition) / 2;
            float ViewHalf = dot(CameraPosition, Half);
            float LightNormal = dot(LightPosition, PointNormal);
            float NormalHalf = pow(dot(PointNormal, Half), 2);

            float D = R / PI * pow((NormalHalf * (R - 1)) + 1, 2);
            float G = G1(PointNormal, CameraPosition, A) + G1(PointNormal, LightPosition, A);
            float3 Ks = Specular + (1 - Specular) * pow(1 - (ViewHalf), 5);

            float3 Fs = D * G * Ks / 4 * ViewNormal * LightNormal;
            float3 Fd = Albedo / PI;

            float3 Fr = ((1 - Ks) * Fd) + Fs;
            Output.xyz += Fr * PL_Dot;
        }
    }*/
            Output = float4(0, 1, 0, 1);

    return Output;
}

