#version 330

uniform mat4 translation;
uniform mat4 scale;

uniform sampler2D tex;
uniform bool mouseOver;

in vec3 position;
in vec3 normal;
in vec2 texcoord;

out vec2 frag_texcoord;

void main(){
	gl_Position = translation * scale * vec4(position,1.0);
	frag_texcoord = texcoord;
}
