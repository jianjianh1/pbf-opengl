#version 460 core

in layout(location = 0) vec2 a_position;

out vec2 v_texCoord;

void main()
{
    gl_Position = vec4(a_position, 1.0 - 1e-3, 1.0);
    v_texCoord = 0.5 * a_position + 0.5;
}