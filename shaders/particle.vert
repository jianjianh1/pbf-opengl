#version 460 core

in layout(location = 0) vec3 a_position;
in layout(location = 1) float a_density;

uniform mat4 u_mvp;
uniform mat4 u_mv;
uniform float u_pointSize;
uniform float u_near;
uniform float u_far;

out vec3 v_positionView;

const float restDensity = 997.0f;

void main()
{
    v_positionView = (u_mv * vec4(a_position, 1.0)).xyz;
    float linearDepth = (-v_positionView.z - u_near) / (u_far - u_near);
    float constraint = a_density / restDensity - 1.0;
    gl_Position = u_mvp * vec4(a_position, 1.0);
    // small size when density is small or depth is far
    gl_PointSize = min(1.0, 1.0 + 10.0 * constraint) * u_pointSize / linearDepth;
}