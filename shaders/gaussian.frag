// reference: https://lisyarus.github.io/blog/graphics/2022/04/21/compute-blur.html

#version 460 core

out layout(location = 0) vec4 out_FragColor;

in vec2 v_texCoord;

uniform sampler2D u_texture;
uniform vec2 u_direction; // already divided by texture size

const int M = 16;

// sigma = 10
// computed by 1 / sqrt(2*pi*sigma**2) * exp(-i**2/(2*sigma**2)) / sum(...)
const float coeffs[M + 1] = {
    0.04426801,
    0.04404722,
    0.04339144,
    0.04232011,
    0.04086452,
    0.03906638,
    0.03697575,
    0.03464877,
    0.03214517,
    0.02952574,
    0.02684991,
    0.02417363,
    0.02154755,
    0.01901565,
    0.01661428,
    0.01437172,
    0.01230816
    };
 
void main()
{
    vec4 sum = coeffs[0] * texture(u_texture, v_texCoord);
    for (int i = 1; i < M; i += 2)
    {
        float w0 = coeffs[i];
        float w1 = coeffs[i + 1];

        float w = w0 + w1;
        float t = w1 / w;

        sum += w * texture(u_texture, v_texCoord + u_direction * (i + t));
        sum += w * texture(u_texture, v_texCoord - u_direction * (i + t));
    }

    out_FragColor = sum;
}
