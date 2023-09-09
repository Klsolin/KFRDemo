#version 410

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;

layout(location = 0) out mediump vec4 fragColor;

uniform sampler2D sTex;

void main()
{
    // fragColor = vec4(normalize(vNormal), 1.0);
    fragColor = texture(sTex, vTexCoord);
}
