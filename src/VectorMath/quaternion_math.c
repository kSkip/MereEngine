#include "VectorMath/quaternion_math.h"
#include <math.h>
#include <string.h>

void quaternion_w(float orientation[4]){

	float t = 1.0f;
	t -= orientation[0]*orientation[0];
	t -= orientation[1]*orientation[1];
	t -= orientation[2]*orientation[2];

	if(t < 0.0f){
		orientation[3] = 0.0f;
	}else{
		orientation[3] = -sqrt(t);
	}

}

void normalize_quaternion(float orientation[4]){

	float factor  = orientation[0]*orientation[0];
	      factor += orientation[1]*orientation[1];
	      factor += orientation[2]*orientation[2];
	      factor += orientation[3]*orientation[3];

	if(factor > 0.0f){

		factor = sqrt(factor);

		orientation[0] /= factor;
		orientation[1] /= factor;
		orientation[2] /= factor;
		orientation[3] /= factor;

	}

}

void quaternion_vector_product(const float quat[4], const float vec[3], float output[4]){

	output[3] = -quat[0]*vec[0] - quat[1]*vec[1] - quat[2]*vec[2];
	output[0] =  quat[3]*vec[0] + quat[1]*vec[2] - quat[2]*vec[1];
	output[1] =  quat[3]*vec[1] + quat[2]*vec[0] - quat[0]*vec[2];
	output[2] =  quat[3]*vec[2] + quat[0]*vec[1] - quat[1]*vec[0];

}

void quaternion_product(const float left[4], const float right[4], float output[4]){

	output[3] = left[3]*right[3] - left[0]*right[0] - left[1]*right[1] - left[2]*right[2];
	output[0] = left[0]*right[3] + left[3]*right[0] + left[1]*right[2] - left[2]*right[1];
	output[1] = left[1]*right[3] + left[3]*right[1] + left[2]*right[0] - left[0]*right[2];
	output[2] = left[2]*right[3] + left[3]*right[2] + left[0]*right[1] - left[1]*right[0];

}

void rotate_position(const float orientation[4], const float position[3], float position_out[3]){

	float temp[4], invQuat[4], quat_out[4];

	invQuat[3] =  orientation[3];
	invQuat[0] = -orientation[0];
	invQuat[1] = -orientation[1];
	invQuat[2] = -orientation[2];

	quaternion_vector_product(orientation,position,temp);

	quaternion_product(temp,invQuat,quat_out);

	memcpy(position_out,quat_out,3*sizeof(float));

}

void interpolate_position(const float floor[3], const float ceil[3], const float interpolation, float output[3]){

	output[0] = floor[0] + interpolation*(ceil[0] - floor[0]);
	output[1] = floor[1] + interpolation*(ceil[1] - floor[1]);
	output[2] = floor[2] + interpolation*(ceil[2] - floor[2]);

}

void interpolate_quaternion(const float floor[4], const float ceil[4], const float interpolation, float output[4]){

	float c, s1, s2, s3, omega;
	float ceil_temp[4];

	c  = floor[0]*ceil[0];
	c += floor[1]*ceil[1];
	c += floor[2]*ceil[2];
	c += floor[3]*ceil[3];

	memcpy(ceil_temp,ceil,4*sizeof(float));

	if(c < 0.0f){

		ceil_temp[0] *= -1.0f;
		ceil_temp[1] *= -1.0f;
		ceil_temp[2] *= -1.0f;
		ceil_temp[3] *= -1.0f;
		c *= -1.0f;

	}

	if(c > 0.9999f){

		output[0] = (1-interpolation)*floor[0] + interpolation*ceil_temp[0];
		output[1] = (1-interpolation)*floor[1] + interpolation*ceil_temp[1];
		output[2] = (1-interpolation)*floor[2] + interpolation*ceil_temp[2];
		output[3] = (1-interpolation)*floor[3] + interpolation*ceil_temp[3];

	}else{

		s1 = sqrt(1.0f - c*c);
		omega = atan2(s1,c);
		s2 = sin((1-interpolation)*omega);
		s3 = sin(interpolation*omega);

		output[0] = s2*floor[0] + s3*ceil_temp[0];
		output[0] /= s1;

		output[1] = s2*floor[1] + s3*ceil_temp[1];
		output[1] /= s1;

		output[2] = s2*floor[2] + s3*ceil_temp[2];
		output[2] /= s1;

		output[3] = s2*floor[3] + s3*ceil_temp[3];
		output[3] /= s1;

	}

}
