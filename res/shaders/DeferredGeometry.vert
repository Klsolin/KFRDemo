#version 410

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

layout(location = 0) out vec3 vPos;
layout(location = 1) out vec2 vTexCoord;
layout(location = 2) out vec3 vNormal;

uniform mat4 uMVPMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;

void main()
{
    vPos = (uModelMatrix * vec4(aPos, 1.0)).xyz;
    vTexCoord = aTexCoord;
    vNormal = uNormalMatrix * aNormal;

    gl_Position = uMVPMatrix * vec4(aPos, 1.0);
}
