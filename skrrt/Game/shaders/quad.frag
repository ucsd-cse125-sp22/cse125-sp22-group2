#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;
uniform sampler2D bloom;
uniform sampler2D uiTex;

uniform float exposure;

void main()
{             
    vec3 result = texture(depthMap, TexCoords).rgb;
    result += texture(bloom, TexCoords).rgb;
    result = vec3(1.0) - exp(-result * exposure);
    
    //vec4 ui = texture(uiTex, TexCoords);
    vec4 ui = vec4(result, 1.0f);
    FragColor = vec4(mix(result, ui.rgb, ui.w), 1.0f); 

}
