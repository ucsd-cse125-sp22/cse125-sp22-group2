#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;
uniform sampler2D bloom;
uniform sampler2D uiTex;
uniform sampler2D dripTex;
uniform sampler2D partTex;

uniform float exposure;

void main()
{             
    vec4 screen = texture(depthMap, TexCoords);
    vec3 result = screen.rgb;
    vec4 part = texture(partTex, TexCoords);
    result += part.rgb * (part.w);
    vec4 drip = texture(dripTex, TexCoords);
    result = mix(result, drip.rgb, clamp(drip.w, 0.0f, 0.80f)); 
    result += texture(bloom, TexCoords).rgb;
    result = vec3(1.0) - exp(-result * exposure);
    
    vec4 ui = texture(uiTex, TexCoords);
    FragColor = vec4(mix(result, ui.rgb, ui.w), 1.0f); 
}
