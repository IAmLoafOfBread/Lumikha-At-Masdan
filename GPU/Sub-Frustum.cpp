#include "GPU.hpp"



void GPUFixedContext::calculate_subFrustum(uint32_t in_index, uint32_t in_multiplier) {
	wait_semaphore(m_cameraSemaphore);

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
	
	const float Ratio = m_cameraData.zFar - m_cameraData.zNear / CASCADED_SHADOW_MAP_COUNT;
	
	const float Near = m_cameraData.zNear + (Ratio * (in_multiplier - 1));
	const float Far = m_cameraData.zNear + (Ratio * in_multiplier);
	
	float4x4 Projection = m_cameraView.projection;
	{
		const float Fz = Far / (Far - Near);
		Projection.vecs[2].z = Fz;
		Projection.vecs[3].z = Fz * -Near;
	}
	invert_matrix(&Projection);
	
	for(uint32_t i = 0; i < CORNER_COUNT; i++) {
		transform_vector4(&Corners[i], &Projection);
		float W = Corners[i].w;
		Corners[i].x /= W;
		Corners[i].y /= W;
		Corners[i].z /= W;
		rotate_vector(reinterpret_cast<float3*>(&Corners[i]), m_cameraView.rotation);
		Corners[i].x += m_cameraView.position.x;
		Corners[i].y += m_cameraView.position.y;
		Corners[i].z += m_cameraView.position.z;
		m_subFrusta[i + (CORNER_COUNT * in_index)].x = Corners[i].x;
		m_subFrusta[i + (CORNER_COUNT * in_index)].y = Corners[i].y;
		m_subFrusta[i + (CORNER_COUNT * in_index)].z = Corners[i].z;
	}

	signal_semaphore(m_cameraSemaphore);
}
