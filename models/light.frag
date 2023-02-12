#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 fogColor;
uniform float density;
uniform float gradient;

uniform sampler2D texture1;

float calculateFog(float zCoord)
{
    float z = abs(zCoord);
    return exp(-density * z * gradient);
}

void main()
{
    vec4 texColor = texture(texture1, TexCoords);
    float fogFactor = calculateFog(gl_FragCoord.z);
    FragColor = mix(texColor, vec4(fogColor, 1.0), fogFactor);
}