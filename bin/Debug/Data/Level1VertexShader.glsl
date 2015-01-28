#version 330

#extension GL_EXT_gpu_shader4 : enable

uniform sampler2D tex;

uniform mat4 MV, P, translation, rotation;
uniform mat4 boneMat[20];
uniform bool skipBones, skipMVP, skipLighting;

in vec3 position;
in vec3 normal;
in vec2 texcoord;
in ivec4 boneIds;
in vec4 boneWeights;

uniform int fades;
uniform float life;

out vec3 fragPosition, fragNormal;
out vec2 fragTexcoord;
out vec4 testColor;


void main(){
	
	mat4 BoneTransform;
	vec4 transformedposition;
	vec4 transformednormal;
	if(!skipBones){
		BoneTransform  = boneMat[boneIds[0]] * boneWeights[0];
		BoneTransform += boneMat[boneIds[1]] * boneWeights[1];
   		BoneTransform += boneMat[boneIds[2]] * boneWeights[2];
    		BoneTransform += boneMat[boneIds[3]] * boneWeights[3];
		transformedposition = translation * rotation * BoneTransform * vec4(position,1.0);
		transformednormal = rotation * BoneTransform * vec4(normal,0.0);
	}else{
		transformednormal = rotation * vec4(normal,0.0);
		transformedposition = translation * rotation * vec4(position,1.0);
	}

	if(!skipMVP){
		gl_Position = P * MV * transformedposition;
	}else{
		gl_Position = transformedposition;
	}
	fragPosition = transformedposition.xyz;
	fragNormal = transformednormal.xyz;
	fragTexcoord = texcoord;

}
