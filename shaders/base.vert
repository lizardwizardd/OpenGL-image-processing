#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;

uniform float scaleDiff;

void main()
{
    gl_Position = vec4(aPos, 0.0f, 1.0f);
    if (scaleDiff >= 1)
        texCoord = aTexCoord * scaleDiff;
    else
        texCoord = aTexCoord;
}
