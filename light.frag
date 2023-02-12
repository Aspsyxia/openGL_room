#version 330 core

in vec3 fragmentPosition;
in vec3 fragmentNormal;

out vec4 fragColor;

uniform sampler2D noiseMap;

uniform vec3 camPos;
uniform float fogStart;
uniform float fogEnd;

vec3 calculateFog()
{
    float dist = length(fragmentPosition - camPos);
    float fogAmount = (dist - fogStart) / (fogEnd - fogStart);
    fogAmount = clamp(fogAmount, 0.0, 1.0);
    vec3 noise = texture(noiseMap, vec2(dist * 0.01, 0.0)).rgb;
    return mix(noise, vec3(1.0), fogAmount);
}

void main()
{
    fragColor = vec4(calculateFog(), 1.0);
}