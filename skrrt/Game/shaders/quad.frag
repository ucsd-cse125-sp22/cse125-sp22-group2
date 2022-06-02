#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;
uniform sampler2D bloom;
uniform sampler2D uiTex;

uniform float exposure;

void main()
{             
    vec4 screen = texture(depthMap, TexCoords);
    vec3 result = screen.rgb;
    result += texture(bloom, TexCoords).rgb;
    result = vec3(1.0) - exp(-result * exposure);
    
    vec4 ui = texture(uiTex, TexCoords);
    FragColor = vec4(mix(result, ui.rgb, ui.w), 1.0f); 
}
