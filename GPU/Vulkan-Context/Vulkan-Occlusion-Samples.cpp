#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_occlusionSamples(void) {
	GPUStageAllocation SampleStage = { GPU_NULL_HANDLE };
	build_stageAllocation(&SampleStage, sizeof(float2) * OCCLUSION_SAMPLE_COUNT);
	
	const float RandMaxLength = static_cast<float>(RAND_MAX);
	const float RandMaxRotation = static_cast<float>(RAND_MAX/6.2831f);
	
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

void GPUFixedContext::ruin_occlusionSamples(void) {
	ruin_localAllocation(&m_occlusionSampleAllocation);
}



#endif
