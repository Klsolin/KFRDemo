#version 410

layout(location = 0) out mediump vec4 fragColor;

uniform vec2 uRes;
uniform vec2 uPos;
uniform float uInnerRadius;
uniform float uOuterRadius;
uniform float uBorder;
uniform float uAntialias;
uniform mediump vec3 uColor;
uniform mediump vec3 uBorderColor;

void main()
{
    float dist = distance(gl_FragCoord.xy, uPos);

    float t =
        mix(1.0,
            smoothstep(uInnerRadius - uAntialias, uInnerRadius + uAntialias, dist) *
            (1.0 - smoothstep(uOuterRadius - uAntialias, uOuterRadius + uAntialias, dist)),
            float(uBorder > 0.0));

    mediump vec3 color = mix(uBorderColor, uColor, t);

    float alpha =
        smoothstep(uInnerRadius - uBorder - uAntialias, uInnerRadius - uBorder, dist) *
        (1.0 - smoothstep(uOuterRadius + uBorder, uOuterRadius + uBorder + uAntialias, dist));

    fragColor = vec4(color, alpha);
}
