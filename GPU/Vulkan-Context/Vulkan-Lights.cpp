#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_lights(void) {
	build_sharedAllocation(&m_lightAllocation, MAX_LIGHT_COUNT * sizeof(Light), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, true);
	m_lights = static_cast<Light*>(m_lightAllocation.data);
}

void GPUFixedContext::ruin_lights(void) {
	ruin_sharedAllocation(&m_lightAllocation);
}



#endif
