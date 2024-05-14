#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform float gamma;
uniform float numColors;

void main()
{
    float numColorsScaled = numColors * 100;

    vec3 col = texture2D(screenTexture, TexCoords).rgb;

    col = pow(col, vec3(gamma, gamma, gamma));
    col = col * numColorsScaled;
    col = floor(col);
    col = col / numColorsScaled;
    col = pow(col, vec3(1.0 / gamma));

    FragColor = vec4(col, 1.0);
}
