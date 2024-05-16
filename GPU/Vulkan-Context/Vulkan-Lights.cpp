#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



static GPUSharedAllocation g_lightAllocation = { nullptr };



void GPUFixedContext::build_lights(void) {
	build_sharedAllocation(&g_lightAllocation, MAX_LIGHT_COUNT * sizeof(Light), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 0);
	m_graphicsLightBuffer = g_lightAllocation.buffer;
	m_lights = static_cast<Light*>(g_lightAllocation.data);
}

void GPUFixedContext::ruin_lights(void) {
	ruin_sharedAllocation(&g_lightAllocation);
}



#endif
