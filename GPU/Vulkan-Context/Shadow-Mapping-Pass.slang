#define SHADER_INCLUDE
#include "../Shader-Utilities.h"



struct Constants {
	let cameraView: View;
	let lightView: View;
}

[[vk::push_constant]] let g_lightCount: Constants;

[shader("vertex")] func output_vertex(in_vertexPosition: float3, in_instancePosition: float3, in_instanceRotation: float3) -> void {

}

