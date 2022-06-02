#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;
uniform sampler2D bloom;

uniform float exposure;

void main()
{             
    vec3 result = texture(depthMap, TexCoords).rgb;
    result += texture(bloom, TexCoords).rgb;
    result = vec3(1.0) - exp(-result * exposure);
    FragColor = vec4(result, 1.0f);;
}
