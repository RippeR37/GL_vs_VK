#version 330 core

layout(location = 0) in vec4 vertexPosition;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vertexPosition;
}
