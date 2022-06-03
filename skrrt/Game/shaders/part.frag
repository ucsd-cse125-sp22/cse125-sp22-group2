#version 330 core

uniform vec4 particleColor; 

// Output the frag color
layout (location = 0) out vec4 fragColor;

void main (void){
	fragColor = particleColor;
}
