#include "GPU.hpp"



void GPUFixedContext::transform_camera(Transform in_transform, float3 in_value) {
	wait_semaphore(m_cameraSemaphore);
	switch(in_transform) {
		case POSITION:
			m_cameraView.instance.position = in_value;
		case ROTATION:
			m_cameraView.instance.rotation = in_value;
	}
	signal_semaphore(m_cameraSemaphore);
}

void GPUFixedContext::update_camera(void) {
	wait_semaphore(m_cameraSemaphore);
	const float XY = 1 / tan(m_cameraData.fov / 2);
	const float Z = m_cameraData.zFar / (m_cameraData.zFar - m_cameraData.zNear);
	const float4x4 Projection = {
		.vecs[0].x = XY * m_surfaceExtent.height / m_surfaceExtent.width,
		.vecs[0].y = 0,
		.vecs[0].z = 0,
		.vecs[0].w = 0,
		.vecs[1].x = 0,
		.vecs[1].y = XY,
		.vecs[1].z = 0,
		.vecs[1].w = 0,
		.vecs[2].x = 0,
		.vecs[2].y = 0,
		.vecs[2].z = Z,
		.vecs[2].w = 1,
		.vecs[3].x = 0,
		.vecs[3].y = 0,
		.vecs[3].z = -Z * m_cameraData.zNear,
		.vecs[3].w = 0
	};
	m_cameraView.projection = Projection;
	signal_semaphore(m_cameraSemaphore);
}
