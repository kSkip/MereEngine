#ifndef BOUNDARY_H
#define BOUNDARY_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include "VectorMath.h"

class projection{
    public:
        projection(){}
        projection(double h,double l) : high(h), low(l){}
        ~projection(){}
        projection(const projection & rhs){high = rhs.high; low = rhs.low;}

        bool overlap(projection p){
            if(p.high >= low && p.low <= high ){
                return true;
            }else{
                return false;
            }
        }

        double getOverlap(projection p){
            if(p.high <= high && p.low >= low){
                return p.high - p.low;
            }else if(p.high >= high && p.low <= low){
                return high-low;
            }else if(p.high >= high && p.low >= low){
                return high-p.low;
            }else if(p.high <= high && p.low <= low){
                return p.high-low;
            }

            return 0;
        }

        double high;
        double low;
};

class block{

public:
	block();
	block(vec3, vec3, vec3, vec3, float, float, float);
	block(const block& rhs);
	~block();

    projection project(vec3, vec3, float);

    vec3 getPosition() const {return position;}

	vec3 position;
	vec3 lengthnorm;
	vec3 widthnorm;
	vec3 heightnorm;

	float length;
	float width;
	float height;

private:

	vec3 vertices[8];

};

class boundary{

public:
	boundary();
	boundary(const boundary& rhs);
	~boundary();

	void LoadBoundaries(std::string filename);
	std::vector<block> blocks;

};

#endif
