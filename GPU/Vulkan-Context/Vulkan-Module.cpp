#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::build_module(GPUModule in_module, const char* in_path) {
	char FullPath[MAX_STRING_LENGTH] = { 0 };
	for(uint32_t i = 0; i < m_directoryLength; i++) {
		FullPath[i] = m_directory[i];
	}
	for (uint32_t i = 0; i < MAX_STRING_LENGTH; i++) {
		char c = in_path[i];
		if(!c) break;
		FullPath[m_directoryLength + i] = c;
	}

	File File = open_file(FullPath);
	const size_t Size = get_fileSize(File);
	FileMap Map = map_file(File);
	void* View = view_fileMap(Map, Size);

	const VkShaderModuleCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.codeSize = Size,
		.pCode = static_cast<uint32_t*>(View)
	};
	CHECK(vkCreateShaderModule(m_logical, &CreateInfo, nullptr, &in_module))

	unview_fileMap(View, Size);
	unmap_file(Map);
	close_file(File);
}

void GPUFixedContext::ruin_module(GPUModule in_module) {
	vkDestroyShaderModule(m_logical, in_module, nullptr);
}



#endif
