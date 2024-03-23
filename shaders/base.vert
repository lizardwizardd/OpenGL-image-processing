#version 330 core

in vec2 vertex;
out vec2 texCoord;

uniform float objectAspectRatio;
uniform float windowAspectRatio;

void main()
{
    vec2 scaledVertex = vertex;

    // Window stretched in width
    if (windowAspectRatio > objectAspectRatio)
    {
        scaledVertex.x *= (windowAspectRatio / objectAspectRatio);
    }
    else // window stretched in height
    {
        scaledVertex.y *= (objectAspectRatio / windowAspectRatio);
    }

    // Transform to [0.0 - 1.0]
    texCoord = (scaledVertex + 1.0) / 2.0;

    gl_Position = vec4(vertex, 0.0, 1.0);
}
