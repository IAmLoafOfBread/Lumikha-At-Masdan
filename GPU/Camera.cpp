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
	m_cameraView.projection.vecs[0].x = XY * m_surfaceExtent.height / m_surfaceExtent.width;
	m_cameraView.projection.vecs[1].y = XY;
	m_cameraView.projection.vecs[2].z = Z;
	m_cameraView.projection.vecs[3].z = -Z * m_cameraData.zNear;
	signal_semaphore(m_cameraSemaphore);
}
