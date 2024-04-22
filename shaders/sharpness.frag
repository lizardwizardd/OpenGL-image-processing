#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float textureWidth;
uniform float textureHeight;
uniform float strength;

vec3 sampleTexture(const float x, const float y)
{
    vec2 uv = TexCoords + vec2(x / textureWidth,
                               y / textureHeight);
    vec3 textureColor = texture2D(screenTexture, uv).rgb;
    return textureColor;
}

vec3 sharpenLaplasian(vec2 fragCoord, float strength)
{
    vec3 f =
        sampleTexture(-1.0, -1.0) * -1.0 +
        sampleTexture(0.0, -1.0)  * -1.0 +
        sampleTexture(1.0, -1.0)  * -1.0 +

        sampleTexture(-1.0, 0.0) * -1.0 +
        sampleTexture(0.0, 0.0)  *  9.0 +
        sampleTexture(1.0, 0.0)  * -1.0 +

        sampleTexture(-1.0, 1.0) * -1.0 +
        sampleTexture(0.0, 1.0)  * -1.0 +
        sampleTexture(1.0, 1.0)  * -1.0;

    return mix(vec4(sampleTexture(0.0, 0.0), 1.0), vec4(f, 1.0), strength).rgb;
}

void main()
{
    vec3 col = sharpenLaplasian(TexCoords, strength);
    FragColor = vec4(col, 1.0);
}
