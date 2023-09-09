#version 410

#define TWO_PI 6.28318530718

layout(location = 0) in vec2 vTexCoord;

layout(location = 0) out mediump vec4 fragColor;

struct Param
{
    float kernelParam;
    float downscaleRatio;
};

uniform Param uParam;
uniform vec2 uGazePoint;
uniform vec2 uRes;

uniform sampler2D sTex;

vec2 transform(vec2 texCoord)
{
    float maxR = max(max(length(vec2(0.0, uRes.y) - uGazePoint),
                         length(vec2(0.0) - uGazePoint)),
                     max(length(uRes - uGazePoint),
                         length(vec2(uRes.x, 0.0) - uGazePoint)));

    vec2 xy = texCoord * uRes - uGazePoint;

    float lr = pow(log(length(xy)) / log(maxR), uParam.kernelParam);
    float theta = atan(xy.y, xy.x) / TWO_PI + step(xy.y, 0.0);

    return vec2(lr, theta) / uParam.downscaleRatio;
}

void main()
{
    fragColor = texture(sTex, transform(vTexCoord));
}
