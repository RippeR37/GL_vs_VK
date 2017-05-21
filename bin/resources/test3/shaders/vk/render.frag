#version 450

layout (location = 0) in vec4 input_color;
layout (location = 1) in vec4 input_normal;
layout (location = 2) in vec4 input_depth_position;

layout (location = 0) out vec4 output_color;

layout (set = 0, binding = 0) uniform sampler2D shadowmap;

const float bias = 0.005;

float PCF()
{
    float shadowComponent = 0.0;
    vec2 texelSize = 0.5 / textureSize(shadowmap, 0);

    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            float currentSampleDepth = texture(shadowmap, input_depth_position.xy + vec2(x, y) * texelSize).r;
            shadowComponent += (input_depth_position.z - bias > currentSampleDepth) ? 0.0 : 1.0;
        }
    }

    return shadowComponent /= 25.0;
}

void main()
{
    float visibility = mix(1.0, PCF(), 0.5);
    output_color = vec4(visibility * input_color.rgb, input_color.a);
}
