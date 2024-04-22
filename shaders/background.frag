#version 460 core

out layout(location = 0) vec4 out_FragColor;

in vec2 v_texCoord;

uniform samplerCube u_skybox;
uniform mat4 u_vpInv;

void main()
{
    vec4 posClip = vec4(v_texCoord * 2.0 - 1.0, 1.0, 1.0);
    vec4 posWorld = u_vpInv * posClip;
    out_FragColor = texture(u_skybox, posWorld.xyz / posWorld.w);
}
