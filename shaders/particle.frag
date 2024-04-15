#version 460 core

out layout(location = 0) vec4 out_FragColor;

in float v_density;

uniform vec4 u_color;

void main()
{
    float alpha = clamp(0.5 * v_density / 997.0, 0.0, 1.0);
    out_FragColor = mix(u_color, vec4(1.0, 0.0, 0.0, 1.0), alpha);
}