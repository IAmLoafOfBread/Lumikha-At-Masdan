#include "../../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_WINDOWS)
#include "../GPU.hpp"



void GPUFixedContext::calculate_subFrustum(float3* in_corners, View* in_view, uint32_t in_multiplier) {
	float4 Corners[CORNER_COUNT] = {
		{-1, 1, 0, 1},
		{-1,-1, 0, 1},
		{ 1, 1, 0, 1},
		{ 1,-1, 0, 1},
		{-1, 1, 1, 1},
		{-1,-1, 1, 1},
		{ 1, 1, 1, 1},
		{ 1,-1, 1, 1}
	};
	
	const float Ratio = m_cameraData.far - m_cameraData.near / CASCADED_SHADOW_MAP_COUNT;
	
	const float Near = m_cameraData.near + (Ratio * (in_multiplier - 1)), Far = m_cameraData.near + (Ratio * in_multiplier);
	
	float4x4 Projection = in_view->projection;
	{
		const float Fz = Far / (Far - Near);
		Projection.vecs[2].z = Fz;
		Projection.vecs[3].z = Fz * -Far;
	}
	invert_matrix(&Projection);
	
	for(uint32_t i = 0; i < CORNER_COUNT; i++) {
		transform_vector(&Corners[i], &Projection);
		float W = Corners[i].w;
		Corners[i].x /= W;
		Corners[i].y /= W;
		Corners[i].z /= W;
		in_corners[i].x = Corners[i].x;
		in_corners[i].y = Corners[i].y;
		in_corners[i].z = Corners[i].z;
	}
}



#endif
