#version 460 core

out layout(location = 0) vec4 out_FragColor;

in vec2 v_texCoord;

uniform vec2 u_texelSize;
uniform sampler2D u_depthMap;
uniform mat4 u_projInv;

vec3 posView(vec2 uv)
{
    float depth = texture(u_depthMap, uv).x;
    vec3 posClip = 2.0 * vec3(uv, depth) - 1.0;
    vec4 _posView = u_projInv * vec4(posClip, 1.0);
    vec3 posView = _posView.xyz / _posView.w;
    return posView;
}

void main()
{
    float depth = texture(u_depthMap, v_texCoord).x;
    if (depth == 1.0)
    {
        discard;
    }

    vec3 center = posView(v_texCoord);
    vec3 right = posView(v_texCoord + vec2(u_texelSize.x, 0.0));
    vec3 left = posView(v_texCoord - vec2(u_texelSize.x, 0.0));
    vec3 up = posView(v_texCoord + vec2(0.0, u_texelSize.y));
    vec3 down = posView(v_texCoord - vec2(0, u_texelSize.y));

    vec3 dx = left - center;
    vec3 dx2 = center - right;
    if (abs(dx.z) > abs(dx2.z))
    {
        dx = dx2; // this makes it better at edges
    }

    vec3 dy = up - center;
    vec3 dy2 = center - down;
    if (abs(dy.z) > abs(dy2.z))
    {
        dy = dy2;
    }

    vec3 normal = normalize(cross(dy, dx));
    out_FragColor = vec4(normal, 1.0);
}