#version 330 core

uniform sampler2D screenTexture;
in vec2 texCoord;
void main()
{
    gl_FragColor = 1 - texture2D(screenTexture, texCoord);
}
