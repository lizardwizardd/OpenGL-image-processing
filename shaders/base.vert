attribute highp vec4 vertex;
varying highp vec2 texCoord;
void main()
{
    gl_Position = vertex;
    texCoord = vertex.xy * 0.5 + 0.5;
}
