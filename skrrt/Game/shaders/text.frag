#version 330 core
in vec2 TexCoords;

uniform sampler2D text;
uniform vec3 textColor;

layout (location = 0) out vec4 color;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}  
