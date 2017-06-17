#version 330 core

layout(location = 0) in vec4 vertexPosition;

out vec4 vColor;

void main()
{
    gl_Position = vertexPosition;
    vColor = vertexPosition;
}
