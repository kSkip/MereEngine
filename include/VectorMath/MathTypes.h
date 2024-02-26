#ifndef MATHTYPES_H
#define MATHTYPES_H

#include <cmath>

struct vec2 {
	union {
		struct { float x, y; };
		struct { float s, t; };
	};
};

struct vec3 {
	union {
		struct { float x, y, z; };
		struct { float r, g, b; };
	};

	inline vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	inline vec3(float x, float y, float z) : x(x), y(y), z(z) {}

	inline vec3 operator+(const vec3& rhs) {
		return vec3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	inline vec3 operator-(const vec3& rhs) {
		return vec3(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	inline vec3 operator*(const float& scalar) {
		return vec3(scalar * x, scalar * y, scalar * z);
	}

	inline vec3& operator+=(const vec3& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
};

struct vec4 {
	union {
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
	};
};

struct quat {
	float x;
	float y;
	float z;
	float w;
};

struct mat4x3 {
	vec3 value[4];

	inline vec3& operator[](int i) {
		return value[i];
	}
};

inline float dot(const vec3& lhs, const vec3& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

inline vec3 operator/(const vec3& lhs, const float& scalar) {
	return vec3(lhs.x / scalar, lhs.y / scalar, lhs.z / scalar);
}

inline vec3 normalize(const vec3& v)
{
	float factor = dot(v, v);
	factor = sqrt(factor);
	return v / factor;
}

inline vec3 cross(const vec3& lhs, const vec3& rhs)
{
	return vec3(
		lhs.y * rhs.z - rhs.y * lhs.z,
		rhs.x * lhs.z - lhs.x * rhs.z,
		lhs.x * rhs.y - rhs.x * lhs.y
	);
}

#endif