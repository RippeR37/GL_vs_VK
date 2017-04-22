#version 450

layout (push_constant) uniform push_constants_t
{
    layout(offset = 0) vec4 position_offset;
} push_constants;

layout (location = 0) in vec4 input_position;

void main()
{
    gl_Position = push_constants.position_offset + vec4(vec3(input_position) * 0.01, 1.0);
}
