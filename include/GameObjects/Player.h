#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include "Camera.h"
#include "Weapon.h"
#include "Boundary.h"

class Player : public GameObject {
public:
	Player();

	void move(double);
	void commitMovement();

	Camera camera;
	Weapon weapon;

	bool holdingForward = false;
	bool holdingBackward = false;
	bool holdingLeftStrafe = false;
	bool holdingRightStrafe = false;

	float movementSpeedFactor;

	boundary bounds;
	boundary* getBounds() { return &bounds; }
};

#endif
