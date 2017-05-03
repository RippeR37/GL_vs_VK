#version 330 core

layout(location = 0) in vec4 vertexPosition;

out vec4 vColor;

void main()
{
    gl_Position = (vertexPosition * vec4(2.0, 2.0, 1.0, 1.0)) - vec4(1.0, 1.0, 0.0, 0.0);
    vColor = vec4(0.4, 0.4, 0.4, 1.0);
}
