#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float exposure;    // [-5; 5] = 0.0
uniform float contrast;    // [0; 2]  = 1.0
uniform float temperature; // [0; 2]  = 0.5
uniform float saturation;  // [0; 2]  = 1.0
uniform float brightness;  // [-1; 1] = 0.0

vec3 adjustBrightness(vec3 color, float brightness) {
    return color + vec3(brightness);
}

vec3 adjustContrast(vec3 color, float contrast) {
    return (color - vec3(0.5)) * contrast + vec3(0.5);
}

vec3 adjustExposure(vec3 col, float exposure) {
    return col * pow(2.0, exposure);
}

vec3 adjustTemperature(vec3 color, float temperature) {
    const vec3 warmColor = vec3(1.0, 0.6, 0.0);
    const vec3 neutralColor = vec3(1.0, 1.0, 1.0);
    const vec3 coolColor = vec3(0.0, 0.4, 1.0);

    vec3 interpolatedColor = mix(warmColor, coolColor, temperature);

    return mix(color, interpolatedColor, temperature / 10000.0);
}

vec3 adjustSaturation(vec3 color, float saturation) {
  vec3 grayscale = vec3(dot(color, vec3(0.2126, 0.7152, 0.0722)));
  return mix(grayscale, color, saturation);
}

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    col = adjustExposure(col, 0);
    col = adjustContrast(col, 1);
    col = adjustTemperature(col, 0.5);
    col = adjustSaturation(col, 1);
    col = adjustBrightness(col, 0);
    FragColor = vec4(col, 1.0);
}
