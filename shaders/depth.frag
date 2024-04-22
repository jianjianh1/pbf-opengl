#version 460 core

in vec3 v_positionView;

uniform mat4 u_proj;
uniform float u_radius;

void main()
{
    vec2 posToCenter = gl_PointCoord * 2.0 - 1.0;
    float distSqrToCenter = dot(posToCenter, posToCenter);
    if (distSqrToCenter > 1.0) discard;
    vec3 normal = vec3(posToCenter, sqrt(1.0 - distSqrToCenter));

    vec3 posView = v_positionView + u_radius * normal;
    vec4 posClip = u_proj * vec4(posView, 1.0);
    // gl_FragDepth = 0.5 * posClip.z / posClip.w + 0.5;
    gl_FragDepth = posClip.z / posClip.w;
}