#version 450

layout(location = 0) in vec4 input_position;
layout(location = 0) out vec4 output_color;

void main()
{
    gl_Position = input_position;
    output_color = input_position;
}
