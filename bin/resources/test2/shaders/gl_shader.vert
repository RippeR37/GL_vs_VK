#version 330 core

layout(location = 0) in vec4 vertexPosition;

out vec4 vColor;

uniform mat4 MVP;

vec4 getHeightColor(float ht)
{
    vec4 result;

    if      (ht <  0.0)  result = vec4(0.0,             0.0,              1.0,  1.0);
    else if (ht <  8.0)  result = vec4(0.0,             ht / 8.0,         0.0,  1.0);
    else if (ht < 16.0)  result = vec4(ht / 8.0 - 1.0,  1.0,              0.0,  1.0);
    else if (ht < 32.0)  result = vec4(1.0,             2.0 - ht / 16.0,  0.0,  1.0);
    else                 result = vec4(1.0,             1.0,              1.0,  1.0);

    return result;
}

void main()
{
    gl_Position = MVP * vertexPosition;
    vColor = getHeightColor(vertexPosition.y);
}
