#include "../GPU/GPU.hpp"



static void resize(GLFWwindow* in_window, int in_width, int in_height) {
	auto Graphics = reinterpret_cast<GPUFixedContext*>(glfwGetWindowUserPointer(in_window));
	Graphics->resize_surfaceUpdate();
}



int tasting(int argc, const char* argv[]) {
	uint32_t PathLength = 0;
	char Path[MAX_STRING_LENGTH] = { 0 };
	
	{
		uint32_t SlashCount = 0;
		for (uint32_t i = 0; i < MAX_STRING_LENGTH; i++) {
			char c = argv[0][i];
			if(c == '/' || c == '\\') {
				SlashCount++;
			}
			if (!c) break;
		}
		for (uint32_t i = 0; i < MAX_STRING_LENGTH; i++) {
			PathLength++;
			Path[i] = argv[0][i];
			if (Path[i] == '/' || Path[i] == '\\') {
				SlashCount--;
			}
			if (!SlashCount) break;
		}
	}
	
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	
	const char* PositionFiles[] = {
		EXECUTABLE_DIRECTORY"Board.pos.objb"
	};
	const char* NormalFiles[] = {
		EXECUTABLE_DIRECTORY"Board.norm.objb"
	};
	const char* UVFiles[] = {
		EXECUTABLE_DIRECTORY"Board.uv.objb"
	};
	const char* IndexFiles[] = {
		EXECUTABLE_DIRECTORY"Board.ind.objb"
	};
	const char* NormalMaps[] = {
		EXECUTABLE_DIRECTORY"Board.norm.png"
	};
	const char* AlbedoMaps[] = {
		EXECUTABLE_DIRECTORY"Board.alb.png"
	};
	const char* SpecularMaps[] = {
		EXECUTABLE_DIRECTORY"Board.spec.png"
	};
	const char* MetalnessMaps[] = {
		EXECUTABLE_DIRECTORY"Board.met.png"
	};
	const char* RoughnessMaps[] = {
		EXECUTABLE_DIRECTORY"Board.rou.png"
	};
	uint32_t InstanceCounts[] = {
		1
	};
	const char** Textures[] = {
		NormalMaps,
		AlbedoMaps,
		SpecularMaps,
		MetalnessMaps,
		RoughnessMaps
	};
	
	GLFWwindow* Window = glfwCreateWindow(520, 520, "Lmbao", nullptr, nullptr);
	auto Graphics = GPUFixedContext(PathLength, Path, Window, LENGTH_OF(InstanceCounts), InstanceCounts, PositionFiles, NormalFiles, UVFiles, IndexFiles, Textures);
	
	Instance Inst = {
		.position = {0, 0, 0},
		.rotation = {0, 0, 0},
		.dataIndex = 0
	};
	Light Ligh = {
		.position = {0, -50, 0},
		.rotation = {1.5707, 0, 0},
		.glare = { 0 },
		.colour = {10000, 10000, 10000},
		.visible = true
	};
	Graphics.add_instance(0, &Inst);
	Inst.position.y = -40;
	//Graphics.add_instance(0, &Inst);
	Graphics.add_light(&Ligh);
	Graphics.m_cameraView.position.y = -25;
	Graphics.m_cameraView.rotation.x = 1.5707;
	
	glfwSetWindowUserPointer(Window, &Graphics);
	glfwSetFramebufferSizeCallback(Window, &resize);
	
	while(!glfwWindowShouldClose(Window)) {
		glfwPollEvents();
		
		Graphics.acquire_nextImageUpdate();
		for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
			Graphics.calculate_subFrustum(i, i + 1);
		}
		Graphics.dispatch_lightViewingUpdate();
		for(uint32_t i = 0; i < CASCADED_SHADOW_MAP_COUNT; i++) {
			Graphics.draw_shadowMappingUpdate(i, i + 1);
		}
		Graphics.draw_geometryUpdate();
		Graphics.draw_lightingUpdate();
		Graphics.submit_presentUpdate();
		//printf("%f %f\n", Graphics.m_lights[0].view.projection.vecs[0].x, Graphics.m_lights[0].view.projection.vecs[1].y);
		//printf("%f %f\n", Graphics.m_lights[0].view.projection.vecs[3].x, Graphics.m_lights[0].view.projection.vecs[3].y);
		//printf("%f\n", Graphics.m_cameraView.projection.vecs[3].z);
		
		//Graphics.m_cameraView.position.x += 0.01f;
		Graphics.m_instances[0].rotation.y += 0.01f;
		Graphics.m_instances[0].rotation.x += 0.01f;
		//if(Graphics.m_instances[0].rotation.y >= 1) {
		//	Graphics.m_instances[0].rotation.y = 1;
		//}
		//Graphics.m_cameraView.rotation.y += 0.01f;
		
		//Graphics.m_lights[0].view.rotation.z += 0.01f;
		//if(Graphics.m_lights[0].view.rotation.x >= 6.283192f) {
		//	Graphics.m_lights[0].view.rotation.x = 0;
		//}
		//Graphics.m_lights[0].view.position.x += 0.04f;
		//printf("%f\n", Graphics.m_lights[0].view.position.x);
		//Graphics.m_instances[1].position.x += 0.04f;
		
		//printf("\n");
		//printf("\n");
		//for(uint32_t i = 0; i < CORNER_COUNT; i++) {
		//	printf("%f %f %f\n", Graphics.m_subFrusta[i].x, Graphics.m_subFrusta[i].y, Graphics.m_subFrusta[i].z);
		//}
		//printf("\n");
		//for(uint32_t i = CORNER_COUNT; i < CORNER_COUNT * 2; i++) {
		//	printf("%f %f %f\n", Graphics.m_subFrusta[i].x, Graphics.m_subFrusta[i].y, Graphics.m_subFrusta[i].z);
		//}
		//printf("\n");
		//for(uint32_t i = CORNER_COUNT * 2; i < CORNER_COUNT * 3; i++) {
		//	printf("%f %f %f\n", Graphics.m_subFrusta[i].x, Graphics.m_subFrusta[i].y, Graphics.m_subFrusta[i].z);
		//}
		//printf("\n");
	}
	
	glfwTerminate();
	return 0;
}
