#version 330 core

in vec3 TexCoords; // вектор направления, таже представляющий трехмерную текстурную координату
uniform samplerCube skybox; // сэмплер для кубической карты

out vec4 FragColor;

void main()
{
    FragColor = texture(skybox, TexCoords);
}
