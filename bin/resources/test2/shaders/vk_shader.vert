#version 450

layout (push_constant) uniform push_constants_t
{
    layout(offset = 0) mat4 MVP;
} push_constants;

layout (location = 0) in vec4 input_position;
layout (location = 1) out vec4 output_color;

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
    gl_Position = push_constants.MVP * input_position;
    output_color = getHeightColor(input_position.y);
}
