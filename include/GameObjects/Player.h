#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include "Camera.h"
#include "Weapon.h"

class Player : public GameObject {
public:
	Player();

	void move(double, Player&);
	void commitMovement();
	void render(Shader&);

	Camera camera;
	Weapon weapon;

	bool holdingForward = false;
	bool holdingBackward = false;
	bool holdingLeftStrafe = false;
	bool holdingRightStrafe = false;

	float movementSpeedFactor;
};

#endif
