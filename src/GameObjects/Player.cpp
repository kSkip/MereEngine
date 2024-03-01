#include "GameObjects/Player.h"

Player::Player() :
	camera(0.0f, 0.1f, 0.0f, 0.1f),
	movementSpeedFactor(1.2f)
{
	position = vec3(-3.0f, 2.0f, 0.0f);
	velocity = vec3(0.0f, 0.0f, 0.0f);
	camera.head = position + vec3(0.0f, 0.3f, 0.0f);

    ground = false;
    destroy = false;
    skipMVP = true;
    skipLighting = true;
    cDistance = 0.0f;
    collisionTypeValue = COLLIDER_COLLIDEE;
}

void Player::move(double deltatime, Player& player) {

	movement = vec3(0.0f, 0.0f, 0.0f);

	// Vector that resolves an invalid attempted movement
	vec4 allowed(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 temp(0.0f, 0.0f, 0.0f, 0.0f);
	vec3 front = camera.getFront();
	vec3 right = camera.getRight();

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
		movement.x += -1 * right.x;
		movement.z += -1 * right.z;
	}
	velocity.y -= GRAVITATIONAL_ACCELERATION * deltatime;

	movement.x = movementSpeedFactor * deltatime * movement.x;
	movement.y = deltatime * velocity.y;
	movement.z = movementSpeedFactor * deltatime * movement.z;

	weapon.move(deltatime);
}

void Player::commitMovement()
{
	position += movement;
	camera.head.x += movement.x;
	camera.head.y += movement.y;
	camera.head.z += movement.z;
	if (movement.y == 0.0f) ground = true;
}

void Player::render(Shader& levelShader) {

	glClear(GL_DEPTH_BUFFER_BIT);

	// TODO: the weapon should really have its own view matrix
	vec3 front = camera.getFront();
	vec3 right = camera.getRight();
	vec3 up = camera.getUp();
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

	weapon.setTransformations(trans, rot);

	weapon.render(levelShader);

}