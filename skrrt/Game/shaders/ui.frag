#version 330 core

in vec4 position; // raw position in the model coord
in vec3 normal;   // raw normal in the model coord
in vec2 TexCoord; // texture coordinates

uniform sampler2D texture_id;

// Output the frag color
out vec4 fragColor;

void main()
{    

	// UI elements 
	fragColor = texture(texture_id, TexCoord);
}  