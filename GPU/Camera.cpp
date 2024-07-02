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
		.vecs[0] = {XY,          0,           0,           0},
		.vecs[1] = { 0,         XY,           0,           0},
		.vecs[2] = { 0,          0,           Z,           1},
		.vecs[3] = { 0,          0, -Z*m_cameraData.zNear,  0},
	};
	m_cameraView.projection = Projection;
	signal_semaphore(m_cameraSemaphore);
}
