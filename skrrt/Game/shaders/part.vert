# version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

uniform mat4 modelview;
uniform mat4 projection;

void main(){
    gl_Position = projection * modelview * vec4( vertex_position, 1.0f );
}
