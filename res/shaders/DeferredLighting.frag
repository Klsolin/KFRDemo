#version 410

#define MAX_LIGHT_COUNT 128

layout(location = 0) in vec2 vTexCoord;

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

uniform vec3 uEyePos;

uniform int uLightCount;
uniform Light uLights[MAX_LIGHT_COUNT];

uniform sampler2D sPosNormalXTex;
uniform sampler2D sSpecMatNormalYTex;
uniform sampler2D sDiffMapShininessMatTex;

void main()
{
    vec4 posNormalX = texture(sPosNormalXTex, vTexCoord);
    vec4 specMatNormalY = texture(sSpecMatNormalYTex, vTexCoord);
    vec4 diffMapShininessMat = texture(sDiffMapShininessMatTex, vTexCoord);

    vec3 pos = posNormalX.xyz;

    float normalX = posNormalX.w;
    float normalY = specMatNormalY.w;
    float normalZ = sqrt(1.0 - normalX * normalX - normalY * normalY);

    vec3 n = vec3(normalX, normalY, normalZ);

    vec3 Ka = vec3(1.0);
    vec3 Kd = vec3(1.0);
    vec3 Ks = specMatNormalY.xyz;
    float shininess = diffMapShininessMat.w * 128.0;

    vec3 Ia = vec3(0.0);
    vec3 Id = vec3(0.0);
    vec3 Is = vec3(0.0);

    for(int i = 0; i < uLightCount; ++i)
    {
        Light light = uLights[i];

        Ia += light.La * Ka;

        vec3 l = light.pos - pos;
        float dist = length(l);
        l /= dist;

        // vec3 l = normalize(-light.dir);

        float dotLN = max(dot(l, n), 0.0);

        if(dotLN > 0.0)
        {
            vec3 v = normalize(uEyePos - pos);
            vec3 h = normalize(l + v);

            float att = 1.0 / (light.att.x + dist * light.att.y + dist * dist * light.att.z);

            Id += light.Ld * Kd * dotLN * att;
            Is += light.Ls * Ks * pow(max(dot(h, n), 0.0), shininess) * att;

            // Id += light.Ld * Kd * dotLN;
            // Is += light.Ls * Ks * pow(max(dot(h, n), 0.0), shininess);
        }
    }

    fragColor = vec4((Ia + Id + Is) * diffMapShininessMat.rgb, 1.0);
}
