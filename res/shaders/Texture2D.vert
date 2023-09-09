#version 410

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

layout(location = 0) out vec2 vTexCoord;

uniform vec2 uRes;
uniform vec2 uPos;
uniform vec2 uSize;

void main()
{
    vec2 scale = uSize / uRes;
    vec2 offset = (uPos / uRes) * 2.0 - 1.0;

    vec2 pos = aPos * scale + offset;

    vTexCoord = (pos + 1.0) * 0.5;

    gl_Position = vec4(pos, 0.0, 1.0);
}
