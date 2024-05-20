#define SHADER_INCLUDE
#include "../Shader-Utilities.h"

struct VSVertexInput {
    [[vk::location(0)]] float3 position;
};

struct VSInstanceInput {
    [[vk::location(1)]] float3 position;
    [[vk::location(2)]] float3 rotation;
};

struct VSOutput {
    float4 viewPosition : SV_POSITION;
};

struct Constants {
    View view;
	Camera camera;
};

[[vk::push_constant]] Constants g_constants;

[shader("vertex")] VSOutput output_vertex(VSVertexInput in_vertex, VSInstanceInput in_instance) {
    VSOutput Output = { 0 };
    float3 Position = in_vertex.position;
    float3 ViewPosition = g_constants.view.position;
    float3 ViewRotation = g_constants.view.rotation;
    Camera Camera = g_constants.camera;

    rotate_vector(Position, in_instance.rotation);
    Position += in_instance.position;
    Position -= ViewPosition;
    rotate_vector(Position, -ViewRotation);
    Camera.position -= ViewPosition;
    rotate_vector(Camera.position, -ViewRotation);
    Camera.rotation -= ViewRotation;

    
    return Output;
}

