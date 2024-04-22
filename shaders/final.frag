#version 460 core

out layout(location = 0) vec4 out_FragColor;

in vec2 v_texCoord;

// the naming is not standard physical quantity but only meaning relevant
struct Fluid
{
    vec3 color;
    float specular;
    float shininess;
    float reflectance;
    float refractance;
    float attenuation;
};

struct Light
{
    vec3 position;
    vec3 specular;
};

uniform sampler2D u_depthMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_thicknessMap;
uniform sampler2D u_background;
uniform samplerCube u_skybox;
uniform mat4 u_projInv;
uniform mat3 u_viewInv;
uniform Fluid u_fluid;
uniform Light u_light;

void main()
{
    float depth = texture(u_depthMap, v_texCoord).x;
    if (depth == 1.0)
    {
        out_FragColor = texture(u_background, v_texCoord);
        return;
    }

    // preparation
    vec3 posClip = 2.0 * vec3(v_texCoord, depth) - 1.0;
    vec4 _posView = u_projInv * vec4(posClip, 1.0);
    vec3 posView = _posView.xyz / _posView.w;
    vec3 eyeDir = normalize(-posView);
    vec3 normal = texture(u_normalMap, v_texCoord).xyz;
    normal = normalize(normal);

    // specular
    vec3 lightDir = normalize(u_light.position - posView);
    vec3 halfway = normalize(eyeDir + lightDir);
    vec3 specular = u_light.specular * u_fluid.specular
                  * pow(max(0.0, dot(halfway, normal)), u_fluid.shininess);

    // reflection
    vec3 reflectDir = reflect(-eyeDir, normal);
    reflectDir = u_viewInv * reflectDir;
    vec3 reflection = texture(u_skybox, reflectDir).xyz;

    // fresnel
    float fresnel = u_fluid.reflectance + (1.0 - u_fluid.reflectance)
                  * pow(max(0.0, dot(normal, eyeDir)), 5.0);

    // refraction
    float thickness = texture(u_thicknessMap, v_texCoord).x;
    float beta = thickness * u_fluid.refractance;
    vec3 refraction = texture(u_background, v_texCoord + beta * normal.xy).xyz;
    float t = exp(-thickness * u_fluid.attenuation);
    vec3 attenuateRefraction = mix(u_fluid.color, refraction, t);

    // modified from curvature flow paper
    vec3 color = mix(attenuateRefraction, reflection, 0.5 * fresnel) + specular;
    out_FragColor = vec4(color, 1.0);
}