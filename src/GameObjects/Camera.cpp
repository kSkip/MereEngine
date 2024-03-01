#include "GameObjects/Camera.h"

const float PI = 3.1415927f;

inline float toRadians(float x)
{
	return (x * 2 * PI) / 360.0f;
}

Camera::Camera(float y, float ySense, float p, float pSense) :
	yaw(y),
	yawSensitivity(ySense),
	pitch(p),
	pitchSensitivity(pSense) {}

void Camera::handleMouseMove(int mouseX, int mouseY)
{
	yaw -= mouseX * yawSensitivity;
	pitch -= mouseY * pitchSensitivity;

	pitch = pitch < 60.0f ? pitch : 60.0f;
	pitch = pitch > -60.0f ? pitch : -60.0f;

	yaw = yaw > 360.0f ? yaw - 360.0f : yaw;
	yaw = yaw < 0.0f ? yaw + 360.0f : yaw;
}

void Camera::getViewMatrix(mat4& view)
{
	float cosYaw = cos(toRadians(yaw));
	float sinYaw = sin(toRadians(yaw));
	float cosPitch = cos(toRadians(pitch));
	float sinPitch = sin(toRadians(pitch));

	right = vec3(cosYaw, 0.0f, -sinYaw);
	up = vec3(sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);
	front = vec3(sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw);

	view[0] = vec4(right.x, up.x, front.x, 0.0f);
	view[1] = vec4(right.y, up.y, front.y, 0.0f);
	view[2] = vec4(right.z, up.z, front.z, 0.0f);
	view[3] = vec4(-dot(right, head), -dot(up, head), -dot(front, head), 1.0f);
}
