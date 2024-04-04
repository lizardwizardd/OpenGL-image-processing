#version 330 core

out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D texture;

void main()
{
    FragColor = texture2D(texture, texCoord) - 0.2f;
}
