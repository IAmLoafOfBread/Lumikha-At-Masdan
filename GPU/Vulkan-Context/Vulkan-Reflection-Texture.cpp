#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_reflectionTexture(void) {
	build_localTexture(&m_reflectionTexture, nullptr, m_surfaceFormat, m_surfaceExtent, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void GPUFixedContext::ruin_reflectionTexture(void) {
	ruin_localTexture(&m_reflectionTexture);
}



#endif
