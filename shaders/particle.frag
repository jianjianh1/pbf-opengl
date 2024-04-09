#version 460 core

out layout(location = 0) vec4 out_FragColor;

uniform vec4 u_color;

void main()
{
    out_FragColor = u_color;
}