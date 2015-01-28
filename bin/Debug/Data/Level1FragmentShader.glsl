#version 330

uniform sampler2D tex;
uniform mat4 MV, P;

uniform int fades;
uniform float life;

uniform bool skipBones, skipMVP, skipLighting;

in vec3 fragPosition, fragNormal;
in vec2 fragTexcoord;
in vec4 testColor;

out vec4 fragColor;

vec3 lightDirection = vec3(0.0,-0.2, 1.0);

const vec4 lightDiffuse = vec4(0.2, 0.2, 0.2, 0.0);
const vec4 lightAmbient = vec4(0.2, 0.2, 0.2, 1.0);
const vec4 lightSpecular = vec4(0.0, 0.0, 0.0, 0.0);

void main(){	
	if(!skipLighting){
		vec3 mvLightDirection = (P * MV * vec4(lightDirection, 0.0)).xyz,
			normal = normalize(fragNormal),
			eye = normalize(fragPosition),
			reflection = reflect(mvLightDirection, normal);

		vec4 fragDiffuse = texture2D(tex, fragTexcoord);

		vec4 diffuseFactor = max(-dot(normal, mvLightDirection), 0.0) * lightDiffuse;

		vec4 ambientDiffuseFactor = diffuseFactor + lightAmbient;


		fragColor = ambientDiffuseFactor * fragDiffuse;
	}else{
		fragColor = texture2D(tex, fragTexcoord);
	}
}
