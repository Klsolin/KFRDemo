#version 410

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

layout(location = 0) out vec3 vPos;
layout(location = 1) out vec2 vTexCoord;
layout(location = 2) out vec3 vNormal;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;

void main()
{
    vPos = aPos;
    vTexCoord = aTexCoord;
    vNormal = aNormal;

    gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(aPos, 1.0);
}
