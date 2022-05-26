#version 330 core

void main (void){
	gl_FragDepth = gl_FragCoord.z;
}
