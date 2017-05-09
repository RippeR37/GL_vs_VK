#version 330 core

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec4 vertexNormal;
layout(location = 2) in vec4 vertexColor;

out vec4 vColor;
out vec4 vNormal;
out vec4 vDepthPosition;

uniform mat4 MVP;
uniform mat4 depthMVP;

void main()
{
    gl_Position = MVP * vertexPosition;
    vDepthPosition = depthMVP * vertexPosition;

    vNormal = vertexNormal;
    vColor = vertexColor;
}
