#version 460 core

out layout(location = 0) vec4 out_FragColor;

in vec2 v_texCoord;

uniform sampler2D u_texture;

void main()
{
     out_FragColor = texture(u_texture, v_texCoord);
    // float depthInv = 1.0 - texture(u_texture, v_texCoord).x;
    // out_FragColor = vec4(depthInv, depthInv, depthInv, 1.0);
}
