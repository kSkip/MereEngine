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

	inline float& operator[](int i) {
		switch (i) {
		default:
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
	}

	inline vec3 operator+(const vec3& rhs) {
		return vec3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	inline vec3 operator-(const vec3& rhs) {
		return vec3(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	inline vec3& operator+=(const vec3& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	inline vec3& operator-=(const vec3& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}
};

struct vec4 {
	union {
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
	};

	inline vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	inline vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	inline float& operator[](int i) {
		switch (i) {
		default:
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		}
	}
};

struct quat {
	float w;
	float x;
	float y;
	float z;

	inline quat() : w(0.0f), x(0.0f), y(0.0f), z(0.0f) {}
	inline quat(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

	inline void calcW() {
		w = 1.0f - x * x - y * y - z * z;
		w = w > 0.0f ? w : 0.0f;
		w = -sqrt(w);
	}

	inline quat operator+(const quat& rhs) {
		return quat(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
	}

	inline vec3 rotate(const vec3&);
};

struct mat3 {
	vec3 value[3];

	inline mat3() {}
	inline mat3(float diag) {
		value[0] = vec3(diag, 0.0f, 0.0f);
		value[1] = vec3(0.0f, diag, 0.0f);
		value[2] = vec3(0.0f, 0.0f, diag);
	}

	inline vec3& operator[](int i) {
		return value[i];
	}

	inline const vec3& operator[](int i) const {
		return value[i];
	}
};

struct mat4 {
	vec4 value[4];

	inline mat4() {}
	inline mat4(float diag) {
		value[0] = vec4(diag, 0.0f, 0.0f, 0.0f);
		value[1] = vec4(0.0f, diag, 0.0f, 0.0f);
		value[2] = vec4(0.0f, 0.0f, diag, 0.0f);
		value[3] = vec4(0.0f, 0.0f, 0.0f, diag);
	}

	inline vec4& operator[](int i) {
		return value[i];
	}
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

inline vec3 operator+(const vec3& lhs, const vec3& rhs) {
	return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

inline vec3 operator*(const vec3& lhs, const float& scalar) {
	return vec3(scalar * lhs.x, scalar * lhs.y, scalar * lhs.z);
}

inline vec3 operator*(const float& scalar, const vec3& rhs) {
	return rhs * scalar;
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

inline quat operator*(const quat& lhs, const float& scalar) {
	return quat(scalar * lhs.w, scalar * lhs.x, scalar * lhs.y, scalar * lhs.z);
}

inline quat operator*(const float& scalar, const quat& rhs) {
	return rhs * scalar;
}

inline quat operator*(const quat& lhs, const quat& rhs) {
	return quat(
		lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z,
		lhs.x * rhs.w + lhs.w * rhs.x + lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.y * rhs.w + lhs.w * rhs.y + lhs.z * rhs.x - lhs.x * rhs.z,
		lhs.z * rhs.w + lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x
	);
}

inline quat operator/(const quat& lhs, const float& scalar) {
	return quat(lhs.w / scalar, lhs.x / scalar, lhs.y / scalar, lhs.z / scalar);
}

inline float dot(const quat& lhs, const quat& rhs)
{
	return lhs.w * rhs.w + lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

inline quat normalize(const quat& v)
{
	float factor = dot(v, v);
	factor = sqrt(factor);
	return v / factor;
}

inline vec3 quat::rotate(const vec3& pos) {
	quat inv(w, -x, -y, -z);
	quat temp(0.0f, pos.x, pos.y, pos.z);
	temp =  *this * temp * inv;
	return vec3(temp.x, temp.y, temp.z);
}

inline quat nlerp(const quat& lhs, const quat& rhs, float t)
{
	return normalize((1.0f - t) * lhs + t * rhs);
}

inline vec3 lerp(const vec3& lhs, const vec3& rhs, float t)
{
	return (1.0f - t) * lhs + t * rhs;
}

inline vec3 operator*(const mat3& lhs, const vec3& rhs)
{
	return rhs.x * lhs[0] + rhs.y * lhs[1] + rhs.z * lhs[2];
}

inline mat4 perspective(float fov, float ar, float nearZ, float farZ)
{
	mat4 p;
	float invTangent = 1.0f / tan(fov / 2);
	p[0][0] = invTangent / ar;
	p[1][1] = invTangent;
	p[2][2] = -(nearZ + farZ) / (farZ - nearZ);
	p[2][3] = -1.0f;
	p[3][2] = 2.0f * farZ * nearZ / (nearZ - farZ);
	return p;
}

inline float toRadians(float x)
{
	static const float PI = 3.1415927f;
	return (x * 2 * PI) / 360.0f;
}

#endif