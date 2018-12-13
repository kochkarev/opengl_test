#version 330 core

layout (location = 0) in vec3 Pos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(Pos, 1.0);
}
