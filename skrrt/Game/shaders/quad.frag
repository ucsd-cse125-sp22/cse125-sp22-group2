#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;

void main()
{             
    vec3 result = vec3(1.0) - exp(-texture(depthMap, TexCoords).rgb);
    FragColor = vec4(result, 1.0f);;
}
