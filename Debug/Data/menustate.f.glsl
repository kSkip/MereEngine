#version 330

uniform mat4 translation;
uniform mat4 scale;

uniform sampler2D tex;
uniform bool mouseOver;

in vec2 frag_texcoord;

out vec4 fragColor;

void main(){
	if(mouseOver == true){
		fragColor = texture2D(tex, frag_texcoord) + vec4(0.5,0.0,0.0,0.0);
	}else{
		fragColor = texture2D(tex, frag_texcoord);
	}
}
