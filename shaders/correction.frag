#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform float exposure;    // [-2; 2] = 0.0
uniform float contrast;    // [0; 2]  = 1.0
uniform float temperature; // [0; 2]  = 0.5
uniform float saturation;  // [0; 2]  = 1.0
uniform float brightness;  // [-1; 1] = 0.0
uniform float tintIntensity;
uniform float filterIntensity;

uniform vec3 tintColor;
uniform vec3 filterColor;

const float LuminancePreservationFactor = 1.0;

vec3 adjustBrightness(vec3 color, float brightness)
{
    return color + vec3(brightness);
}

vec3 adjustContrast(vec3 color, float contrast)
{
    return (color - vec3(0.5)) * contrast + vec3(0.5);
}

vec3 adjustExposure(vec3 col, float exposure)
{
    return col * pow(2.0, exposure);
}

vec3 colorTemperatureToRGB(const in float temperature)
{
    mat3 m = (temperature <= 6500.0) ?
                mat3(vec3(0.0, -2902.1955, -8257.7997),
                     vec3(0.0, 1669.5803,   2575.2827),
                     vec3(1.0, 1.3302,      1.8993)) :

                mat3(vec3( 1745.0425,  1216.6168, -8257.7997),
                     vec3(-2666.3474, -2173.1012,  2575.2827),
                     vec3( 0.5599,     0.7038,     1.8993));
  return mix(clamp(vec3(m[0] / (vec3(clamp(temperature, 1000.0, 40000.0)) + m[1]) + m[2]),
             vec3(0.0), vec3(1.0)), vec3(1.0), smoothstep(1000.0, 0.0, temperature));
}

vec3 adjustTemperature(vec3 color, float temperatureInput)
{
    // Map [-1, 1] to [1000, 40000]
    float temperatureValue = mix(1000.0, 12000.0, (temperatureInput + 1) * 0.5);
    vec3 outColor = mix(color, color * colorTemperatureToRGB(temperatureValue), 1.0);
    outColor *= mix(1.0, dot(color, vec3(0.2126, 0.7152, 0.0722)) / max(dot(outColor,
                    vec3(0.2126, 0.7152, 0.0722)), 1e-5), LuminancePreservationFactor);
  return outColor;
}

vec3 adjustSaturation(vec3 color, float saturation)
{
    vec3 grayscale = vec3(dot(color, vec3(0.2126, 0.7152, 0.0722)));
    return mix(grayscale, color, saturation);
}

vec3 adjustTint(vec3 color, vec3 tintColor, float tintIntensity)
{
    vec3 tintedColor = mix(color, mix(vec3(dot(color, vec3(0.2126, 0.7152, 0.0722))),
                       tintColor, tintIntensity), tintIntensity);
    return tintedColor;
}

void main()
{
    vec3 col = texture2D(screenTexture, TexCoords).rgb;
    col = mix(col, col * filterColor, filterIntensity);
    col = adjustTint(col, tintColor, tintIntensity);
    col = adjustTemperature(col, temperature);
    col = adjustExposure(col, exposure);
    col = adjustContrast(col, contrast);
    col = adjustSaturation(col, saturation);
    col = adjustBrightness(col, brightness);

    FragColor = vec4(col, 1.0);
}
