#version 450

layout (push_constant) uniform push_constants_t
{
    layout(offset = 0) mat4 MVP;
} push_constants;

layout (location = 0) in vec4 input_position;

void main()
{
    gl_Position = push_constants.MVP * input_position;
}
