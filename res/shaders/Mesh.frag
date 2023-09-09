#version 410

layout(location = 0) in vec2 vTexCoord;

layout(location = 0) out mediump vec4 fragColor;

uniform sampler2D sTex;

void main()
{
    fragColor = texture(sTex, vTexCoord);
}
