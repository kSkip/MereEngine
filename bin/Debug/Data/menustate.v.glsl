#version 330

uniform mat4 translation;
uniform mat4 scale;

uniform sampler2D tex;
uniform bool mouseOver;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

layout(location = 3) out vec2 frag_texcoord;

void main(){
	gl_Position = translation * scale * vec4(position,1.0);
	frag_texcoord = texcoord;
}
