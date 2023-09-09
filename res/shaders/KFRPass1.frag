#version 410

#define TWO_PI 6.28318530718

layout(location = 0) in vec2 vTexCoord;

layout(location = 0) out vec4 oPosNormalX;
layout(location = 1) out vec4 oSpecMatNormalY;
layout(location = 2) out mediump vec4 oDiffMapShininessMat;

struct Param
{
    float kernelParam;
    float downscaleRatio;
};

uniform Param uParam;
uniform vec2 uGazePoint;
uniform vec2 uRes;

uniform sampler2D sPosNormalXTex;
uniform sampler2D sSpecMatNormalYTex;
uniform sampler2D sDiffMapShininessMatTex;

vec2 transform(vec2 texCoord)
{
    float maxR = max(max(length(vec2(0.0, uRes.y) - uGazePoint),
                         length(vec2(0.0) - uGazePoint)),
                     max(length(uRes - uGazePoint),
                         length(vec2(uRes.x, 0.0) - uGazePoint)));

    vec2 uv = texCoord * uParam.downscaleRatio;
    uv = vec2(pow(uv.x, 1.0 / uParam.kernelParam), uv.y * TWO_PI);

    float l = exp(uv.x * log(maxR));
    vec2 xy = vec2(l * cos(uv.y), l * sin(uv.y));

    return (xy + uGazePoint) / uRes;
}

void main()
{
    vec2 newTexCoord = transform(vTexCoord);

    oPosNormalX = texture(sPosNormalXTex, newTexCoord);
    oSpecMatNormalY = texture(sSpecMatNormalYTex, newTexCoord);
    oDiffMapShininessMat = texture(sDiffMapShininessMatTex, newTexCoord);
}
