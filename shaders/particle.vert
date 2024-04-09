#version 460 core

in layout(location = 0) vec4 a_position;

uniform mat4 u_mvp;

void main()
{
    gl_Position = u_mvp * a_position;
}