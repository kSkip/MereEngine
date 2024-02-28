#include "GameObjects/Camera.h"

Camera::Camera(ObjectData* objectData, ObjectData* weaponData)
{

	position = vec3(-3.0f, 2.0f, 0.0f);
	velocity = vec3(0.0f, 0.0f, 0.0f);
	head.x = position.x + 0.0f;
	head.y = position.y + 0.3f;
	head.z = position.z + 0.0f;

	yaw = 0.0f;
	pitch = 0.0f;

	movementSpeedFactor = 1.2f;

	pitchSensitivity = 0.1f;
	yawSensitivity = 0.1f;

	ground = false;

	holdingForward = false;
	holdingBackward = false;
	holdingLeftStrafe = false;
	holdingRightStrafe = false;

	data = objectData;

    currentWeapon = new Weapon(weaponData);

    destroy = false;

    skipMVP = true;
    skipLighting = true;

    cDistance = 0.0f;

    collisionTypeValue = COLLIDER_COLLIDEE;
}

void Camera::handleMouseMove(int mouseX, int mouseY)
{
	yaw -= mouseX * yawSensitivity;
	pitch -= mouseY * pitchSensitivity;

	pitch = pitch < 60.0f ? pitch : 60.0f;
	pitch = pitch > -60.0f ? pitch : -60.0f;

	yaw = yaw > 360.0f ? yaw - 360.0f : yaw;
	yaw = yaw < 0.0f ? yaw + 360.0f : yaw;
}

// Function to calculate which direction we need to move the camera and by what amount
void Camera::move(double deltatime, Camera* player){

	movement = vec3(0.0f,0.0f,0.0f);

	// Vector that resolves an invalid attempted movement
	vec4 allowed(0.0f,0.0f,0.0f,0.0f);
	vec4 temp(0.0f,0.0f,0.0f,0.0f);

	if (holdingForward)
	{
		movement.x += -front.x;
		movement.z += -front.z;
	}

	if (holdingBackward)
	{
		movement.x += front.x;
		movement.z += front.z;
	}

	if (holdingRightStrafe)
	{
		movement.x += right.x;
		movement.z += right.z;
	}

	if (holdingLeftStrafe)
	{
		movement.x += -1*right.x;
		movement.z += -1*right.z;
	}
	velocity.y -= GRAVITATIONAL_ACCELERATION*deltatime;

	movement.x = movementSpeedFactor * deltatime * movement.x;
    movement.y = deltatime * velocity.y;
    movement.z = movementSpeedFactor * deltatime * movement.z;

	currentWeapon->move(deltatime, player);
}

void Camera::getViewMatrix(mat4& view)
{
	float cosYaw = cos((yaw * 2 * PI) / 360.0f);
	float sinYaw = sin((yaw * 2 * PI) / 360.0f);
	float cosPitch = cos((pitch * 2 * PI) / 360.0f);
	float sinPitch = sin((pitch * 2 * PI) / 360.0f);

	right = vec3(cosYaw, 0.0f, -sinYaw);
	up = vec3(sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);
	front = vec3(sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw);

	view[0] = vec4(right.x, up.x, front.x, 0.0f);
	view[1] = vec4(right.y, up.y, front.y, 0.0f);
	view[2] = vec4(right.z, up.z, front.z, 0.0f);
	view[3] = vec4(-dot(right, head), -dot(up, head), -dot(front, head), 1.0f);
}

void Camera::render(Shader& levelShader){

    glClear(GL_DEPTH_BUFFER_BIT);

	// TODO: the weapon should really have its own view matrix
	vec3 pos = position + 0.1f * front + 0.2f * right;
    mat4 trans(1.0f);
	trans[3].x = pos.x;
	trans[3].y = pos.y;
	trans[3].z = pos.z;

	mat4 rot;
	rot[0] = vec4(-right.x, -right.y, -right.z, 0.0f);
	rot[1] = vec4(up.x, up.y, up.z, 0.0f);
	rot[2] = vec4(-front.x, -front.y, -front.z, 0.0f);
	rot[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    currentWeapon->setTransformations(trans,rot);

    currentWeapon->render(levelShader);

}
