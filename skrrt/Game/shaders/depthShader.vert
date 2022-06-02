# version 330 core

layout (location = 0) in vec3 vertex_position;

uniform mat4 model;
uniform mat4 lightSpace;

void main(){
    gl_Position = lightSpace * model * vec4( vertex_position, 1.0f );
}
