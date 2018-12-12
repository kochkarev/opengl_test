#version 330 core

layout (location = 0) in vec3 Pos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = Pos;
    gl_Position = projection * view * vec4(Pos, 1.0);
}
