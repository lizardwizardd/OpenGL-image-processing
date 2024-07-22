#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float textureHeight;
uniform float textureWidth;
uniform float pixelSize; // 1 - 64

void main()
{
    float pixelSizeScaled = pixelSize * 100;
    vec2 pixelatedCoords = vec2(floor(TexCoords.x * textureWidth / pixelSizeScaled) * pixelSizeScaled / textureWidth,
                                floor(TexCoords.y * textureHeight / pixelSizeScaled) * pixelSizeScaled / textureHeight);
    vec3 col = texture(screenTexture, pixelatedCoords).rgb;
    FragColor = vec4(col, 1.0);
}
