#version 460 core

in layout(location = 0) vec3 a_position;
in layout(location = 1) float a_density;

out float v_density;

uniform mat4 u_mvp;

void main()
{
    gl_Position = u_mvp * vec4(a_position, 1.0);
    v_density = a_density;
}