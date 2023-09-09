#version 410

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;

layout(location = 0) out vec4 oPosNormalX;
layout(location = 1) out vec4 oSpecMatNormalY;
layout(location = 2) out mediump vec4 oDiffMapShininessMat;

struct Material
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
};

uniform Material uMat;

uniform sampler2D sDiffMapTex;

void main()
{
    vec3 normal = normalize(vNormal);

    oPosNormalX = vec4(vPos, normal.x);
    oSpecMatNormalY = vec4(uMat.Ks, normal.y);
    oDiffMapShininessMat = vec4(texture(sDiffMapTex, vTexCoord).rgb, uMat.shininess / 128.0);
}
