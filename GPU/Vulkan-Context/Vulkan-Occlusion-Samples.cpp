#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



#define TAU 6.2831f



void GPUFixedContext::build_lightingSamples(void) {
	GPUStageAllocation SampleStage = { GPU_NULL_HANDLE };
	{
		build_stageAllocation(&SampleStage, sizeof(float2) * REFLECTION_SAMPLE_COUNT);
		
		float2* Samples = static_cast<float2*>(SampleStage.data);
		float IncRot = 0.0f;
		for(uint32_t i = 0; i < OCCLUSION_SAMPLE_COUNT; i++) {
			float3 Vector = {1.5f, 0.0f, 0.0f};
			float3 Rotation = {0.0f, 0.0f, IncRot};
			rotate_vector(&Vector, Rotation);
			Samples[i].v = Vector.y / m_surfaceExtent.height;
			Samples[i].u = Vector.x / m_surfaceExtent.width;
			IncRot += TAU / REFLECTION_SAMPLE_COUNT;
		}
		build_localAllocation(&m_reflectionSampleAllocation, &SampleStage, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
	}
	{
		build_stageAllocation(&SampleStage, sizeof(float2) * OCCLUSION_SAMPLE_COUNT);
		const float RandMaxLength = static_cast<float>(RAND_MAX);
		const float RandMaxRotation = static_cast<float>(RAND_MAX/TAU);
		
		float2* Samples = static_cast<float2*>(SampleStage.data);
		for(uint32_t i = 0; i < OCCLUSION_SAMPLE_COUNT; i++) {
			float3 Vector = {static_cast<float>(rand()) / RandMaxLength, 0.0f, 0.0f};
			float3 Rotation = {0.0f, 0.0f, static_cast<float>(rand()) / RandMaxRotation};
			rotate_vector(&Vector, Rotation);
			Samples[i].u = (Vector.x / m_surfaceExtent.width) * 4.0f;
			Samples[i].v = (Vector.y / m_surfaceExtent.height) * 4.0f;
		}
		build_localAllocation(&m_occlusionSampleAllocation, &SampleStage, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
	}
}

void GPUFixedContext::ruin_lightingSamples(void) {
	ruin_localAllocation(&m_reflectionSampleAllocation);
	ruin_localAllocation(&m_occlusionSampleAllocation);
}



#endif
