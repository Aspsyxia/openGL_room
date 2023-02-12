#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 fragmentPosition;
out vec3 fragmentNormal;

uniform mat4 model;
uniform vec3 camPos;

void main()
{
    gl_Position = model * vec4(aPos, 1.0);
    fragmentPosition = aPos;
    fragmentNormal = aNormal;
}