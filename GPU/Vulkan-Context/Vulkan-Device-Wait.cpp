#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::await_device(void) {
	CHECK(vkDeviceWaitIdle(m_logical))
}



#endif
