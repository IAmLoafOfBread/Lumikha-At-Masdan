#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::resize_surfaceUpdate(void) {
	vkDeviceWaitIdle(m_logical);
	
	ruin_lightingSamples();
	ruin_geometryFramebuffer();
	ruin_lightingFramebuffer();
	ruin_postProcessingFramebuffers();
	ruin_geometryPipeline();
	ruin_lightingPipeline();
	ruin_postProcessingPipeline();
	ruin_swapchain();
	ruin_surface();

	build_surface();
	build_swapchain();
	build_geometryPipeline();
	build_lightingPipeline();
	build_postProcessingPipeline();
	build_geometryFramebuffer();
	build_lightingFramebuffer();
	build_postProcessingFramebuffers();
	build_lightingSamples();
	set_lightingBindings();
	set_postProcessingBindings();
	initialize_geometryUpdateData();
	initialize_lightingUpdateData();
	update_camera();
}



#endif
