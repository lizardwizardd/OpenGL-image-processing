#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform float pixelSize;

void main()
{
    vec2 pixelCoords = floor(TexCoords * pixelSize) / pixelSize;
    vec3 color = texture(screenTexture, pixelCoords).rgb;
    FragColor = vec4(color, 1.0);
}
