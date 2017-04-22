#version 450

layout (push_constant) uniform push_constants_t
{
    layout(offset = 16) vec4 color;
} push_constants;

layout (location = 0) out vec4 output_color;

void main()
{
    output_color = push_constants.color;
}
