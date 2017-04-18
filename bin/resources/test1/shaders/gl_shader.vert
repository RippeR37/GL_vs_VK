#version 330 core

layout(location = 0) in vec4 vertexPosition;

out vec4 vColor;

uniform vec4 objPosition;
uniform vec4 objColor;

void main()
{
    gl_Position = objPosition + (vertexPosition * 0.01);
    vColor = objColor;
}
