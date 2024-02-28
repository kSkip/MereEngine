#include "Boundary.h"

boundary::boundary(){}

boundary::boundary(const boundary& rhs){}

boundary::~boundary(){}

void boundary::LoadBoundaries(std::string filename){
	std::ifstream ifs;
	char t;
	float values[6];
	vec3 position, lengthnorm, widthnorm, heightnorm;

	ifs.open(filename.c_str());

	while(ifs >> t){
		switch(t){
			case 'b':
				ifs >> values[0] >> values[1] >> values[2];
				position = vec3(values[0],values[1],values[2]);

				ifs >> values[0] >> values[1] >> values[2];
				lengthnorm = vec3(values[0],values[1],values[2]);

				ifs >> values[0] >> values[1] >> values[2];
				widthnorm = vec3(values[0],values[1],values[2]);

				ifs >> values[0] >> values[1] >> values[2];
				heightnorm = vec3(values[0],values[1],values[2]);

				ifs >> values[0] >> values[1] >> values[2];
				blocks.push_back(block(position,lengthnorm,widthnorm,heightnorm,values[0],values[1],values[2]));
				break;
		}
	}

	ifs.close();

}

block::block(){}

block::block(vec3 pos, vec3 lnorm, vec3 wnorm, vec3 hnorm, float l, float w, float h){
	position = pos;
	lengthnorm = normalize(lnorm);
	widthnorm = normalize(wnorm);
	heightnorm = normalize(hnorm);

	length = l;
	width = w;
	height = h;

	vertices[0] = (length*lengthnorm + width*widthnorm + height*heightnorm)/2.0f;
    vertices[1] = (length*lengthnorm - width*widthnorm + height*heightnorm)/2.0f;
    vertices[2] = (length*lengthnorm + width*widthnorm - height*heightnorm)/2.0f;
    vertices[3] = (length*lengthnorm - width*widthnorm - height*heightnorm)/2.0f;
    vertices[4] = (-1.0f*length*lengthnorm + width*widthnorm + height*heightnorm)/2.0f;
    vertices[5] = (-1.0f*length*lengthnorm - width*widthnorm + height*heightnorm)/2.0f;
    vertices[6] = (-1.0f*length*lengthnorm + width*widthnorm - height*heightnorm)/2.0f;
    vertices[7] = (-1.0f*length*lengthnorm - width*widthnorm - height*heightnorm)/2.0f;

}

block::block(const block& rhs){
	position = rhs.position;
	lengthnorm = rhs.lengthnorm;
	widthnorm = rhs.widthnorm;
	heightnorm = rhs.heightnorm;

	length = rhs.length;
	width = rhs.width;
	height = rhs.height;

	vertices[0] = rhs.vertices[0];
	vertices[1] = rhs.vertices[1];
	vertices[2] = rhs.vertices[2];
	vertices[3] = rhs.vertices[3];
	vertices[4] = rhs.vertices[4];
	vertices[5] = rhs.vertices[5];
	vertices[6] = rhs.vertices[6];
	vertices[7] = rhs.vertices[7];
}

block::~block(){}

projection block::project(vec3 axis, vec3 objectPosition, float roty){

    unsigned int i;

	float sinRotY = sin(roty);
	float cosRotY = cos(roty);
	mat3 rotMat = mat3(1.0f);
	rotMat[0][0] = cosRotY;
	rotMat[0][2] = -sinRotY;
	rotMat[2][0] = sinRotY;
	rotMat[2][2] = cosRotY;
	double min = dot(axis, rotMat * (vertices[0] + this->position) + objectPosition);
    double max = min;

    for(i=1;i<8;i++){
		double p = dot(axis, rotMat * (vertices[i] + this->position) + objectPosition);
        if(p < min){
            min = p;
        }else if(p > max){
            max = p;
        }
    }

    return projection(max,min);

}
