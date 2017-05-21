#version 450

layout(location = 0) out float output_depth;

void main()
{
    output_depth = gl_FragCoord.z;
}
