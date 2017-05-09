#version 330 core

in vec4 vColor;
in vec4 vNormal;
in vec4 vDepthPosition;
out vec4 fColor;

uniform sampler2D shadowMap;

const float bias = 0.005;

float PCF()
{
    float shadowComponent = 0.0;
    vec2 texelSize = 0.5 / textureSize(shadowMap, 0);

    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            float currentSampleDepth = texture(shadowMap, vDepthPosition.xy + vec2(x, y) * texelSize).r;
            shadowComponent += (vDepthPosition.z - bias > currentSampleDepth) ? 0.0 : 1.0;
        }
    }

    return shadowComponent /= 25.0;
}

void main()
{
    float visibility = mix(1.0, PCF(), 0.5);
    fColor = vec4(visibility * vColor.rgb, vColor.a);
}
