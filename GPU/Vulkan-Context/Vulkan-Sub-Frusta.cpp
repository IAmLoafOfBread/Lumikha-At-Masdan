#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_subFrusta(void) {
	build_sharedAllocation(&m_subFrustumAllocation, sizeof(float3) * CORNER_COUNT * CASCADED_SHADOW_MAP_COUNT, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, true);
}

void GPUFixedContext::ruin_subFrusta(void) {
	ruin_sharedAllocation(&m_subFrustumAllocation);
}



#endif
