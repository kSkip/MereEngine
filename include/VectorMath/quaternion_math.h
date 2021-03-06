#ifndef QUATERNION_MATH_H
#define QUATERNION_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

void quaternion_w(float orientation[4]);

void normalize_quaternion(float orientation[4]);

void quaternion_vector_product(const float quat[4], const float vec[3], float output[4]);

void quaternion_product(const float left[4], const float right[4], float output[4]);

void rotate_position(const float orientation[4], const float position[3], float position_out[3]);

void interpolate_position(const float floor[3], const float ceil[3], float interpolation, float output[3]);

void interpolate_quaternion(const float floor[4], const float ceil[4], const float interpolation, float output[4]);

#ifdef __cplusplus
}
#endif

#endif
