#version 330 core

in vec3 textureDir; // вектор направления, таже представляющий трехмерную текстурную координату
uniform samplerCube skybox; // сэмплер для кубической карты

void main()
{
    FragColor = texture(skybox, textureDir);
}
