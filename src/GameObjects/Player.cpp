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

void Player::move(double deltatime) {

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
