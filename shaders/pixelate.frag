#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform float pixelSize;

void main()
{
    //vec2 pixelCoords = floor(TexCoords * pixelSize) / pixelSize;
    vec3 color = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(1.0 - color, 1.0);
}
