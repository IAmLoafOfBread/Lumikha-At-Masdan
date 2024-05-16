#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"




void GPUFixedContext::build_module(GPUModule in_module, const char* in_path) {
	int File = open(in_path, O_RDONLY);
	size_t Size = 0; {
		struct stat Status = {};
		fstat(File, &Status);
		Size = Status.st_size;
	}
	auto Buffer = static_cast<uint32_t*>(mmap(nullptr, Size, PROT_READ, MAP_PRIVATE, File, 0));
	const VkShaderModuleCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.codeSize = Size,
		.pCode = Buffer
	};
	CHECK(vkCreateShaderModule(m_logical, &CreateInfo, nullptr, &in_module))
	munmap(Buffer, Size);
	close(File);
}

void GPUFixedContext::ruin_module(GPUModule in_module) {
	vkDestroyShaderModule(m_logical, in_module, nullptr);
}



#endif
