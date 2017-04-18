#version 450

/*
layout (binding = 0) uniform UBO_DATA
{
    vec4 color;
} ubo_data;
*/

layout (location = 0) in vec4 input_position;
layout (location = 0) out vec4 output_color;

void main()
{
    gl_Position = vec4(vec3(input_position) * 0.01, 1.0);
    output_color = vec4(0.0, 0.5, 1.0, 1.0);
}
