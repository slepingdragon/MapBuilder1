#version 330 core

layout(location = 0) in vec2 aPos;       // vertex position (x, y)
layout(location = 1) in vec2 aTexCoord;  // texture coordinates (u, v)

uniform mat4 uProjection;

out vec2 TexCoord;

void main()
{
    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}