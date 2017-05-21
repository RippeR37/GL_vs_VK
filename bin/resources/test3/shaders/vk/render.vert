#version 450

layout (push_constant) uniform push_constants_t
{
    mat4 MVP;
    mat4 depthMVP;
} push_constants;

layout (location = 0) in vec4 input_position;
layout (location = 1) in vec4 input_normal;
layout (location = 2) in vec4 input_color;

layout (location = 0) out vec4 output_color;
layout (location = 1) out vec4 output_normal;
layout (location = 2) out vec4 output_depth_position;

void main()
{
    gl_Position = push_constants.MVP * input_position;
    output_depth_position = push_constants.depthMVP * input_position;

    output_color = input_color;
    output_normal = input_normal;
}
