#version 410

#define MAX_LIGHT_COUNT 128

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;

layout(location = 0) out mediump vec4 fragColor;

struct Light
{
    vec3 pos;
    // vec3 dir;
    vec3 La;
    vec3 Ld;
    vec3 Ls;
    vec3 att;
};

struct Material
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
};

uniform vec3 uEyePos;

uniform int uLightCount;
uniform Light uLights[MAX_LIGHT_COUNT];

uniform Material uMat;

uniform sampler2D sTex;

void main()
{
    vec3 n = normalize(vNormal);

    vec3 Ia = vec3(0.0);
    vec3 Id = vec3(0.0);
    vec3 Is = vec3(0.0);

    for(int i = 0; i < uLightCount; ++i)
    {
        Light light = uLights[i];

        Ia += light.La * 1.0/* uMat.Ka */;

        vec3 l = light.pos - vPos;
        float dist = length(l);
        l /= dist;

        // vec3 l = normalize(-light.dir);

        float dotLN = max(dot(l, n), 0.0);

        if(dotLN > 0.0)
        {
            vec3 v = normalize(uEyePos - vPos);
            vec3 h = normalize(l + v);

            float att = 1.0 / (light.att.x + dist * light.att.y + dist * dist * light.att.z);

            Id += light.Ld * 1.0/* uMat.Kd */ * dotLN * att;
            Is += light.Ls * uMat.Ks * pow(max(dot(h, n), 0.0), uMat.shininess) * att;

            // Id = light.Ld * 1.0/* uMat.Kd */ * dotLN;
            // Is = light.Ls * uMat.Ks * pow(max(dot(h, n), 0.0), uMat.shininess);
        }
    }

    fragColor = vec4((Ia + Id + Is) * texture(sTex, vTexCoord).rgb, 1.0);
}
