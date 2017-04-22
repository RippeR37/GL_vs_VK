#version 330 core

layout(location = 0) in vec4 vertexPosition;

out vec4 vColor;

uniform vec4 objPosition;
uniform vec4 objColor;

void main()
{
    gl_Position = objPosition + vec4(vec3(vertexPosition) * 0.01, 1.0);
    vColor = objColor;
}
