#include "GameObjects/Camera.h"
#include "Utilities/TextManipulation.h"
#include "Utilities/DataBlock.h"

Camera::Camera(ObjectData* objectData, ObjectData* weaponData, float windowSize[2], DataBlock& def){

	initCamera();

    strvec pos = split(def("position"),',');

    if(pos.size() != 4)
        throw std::runtime_error("Position requires 4 values");

    float pos_init[3];
    pos_init[0] = atof(pos[0].c_str());
    pos_init[1] = atof(pos[1].c_str());
    pos_init[2] = atof(pos[2].c_str());

    rotY  = atof(pos[3].c_str());
    rotY  = -rotY * 2.0f * PI / 360.0f;

	position = glm::vec3(pos_init[0],pos_init[1],pos_init[2]);
	head     = position + glm::vec3(0.0f,0.3f,0.0f);
	origin   = head     + glm::vec3(0.0f,0.0f,1000.0f);

	origin = glm::rotateY(origin-head,-1*rotY);
	origin = origin + head;

	front = glm::rotateY(front,-1*rotY);
	right = glm::rotateY(right,-1*rotY);

	data = objectData;

    currentWeapon = new Weapon(weaponData);

    destroy = false;

    skipMVP = true;
    skipLighting = true;

    cDistance = 0.0f;

    collisionTypeValue = COLLIDER_COLLIDEE;
}

Camera::~Camera(){}

void Camera::initCamera(){

	position = glm::vec3(0.0f,0.0f,0.0f);
	head     = glm::vec3(0.0f,0.0f,0.0f);
	origin   = glm::vec3(0.0f,0.0f,0.0f);
	front    = glm::vec3(0.0f,0.0f,1.0f);
	right    = glm::vec3(-1.0f,0.0f,0.0f);
	up       = glm::vec3(0.0f, 1.0f, 0.0f);
	velocity = glm::vec3(0.0f,0.0f,0.0f);

    rotHorizontal = 0.0f;
    rotVertical = 0.0f;

	movementSpeedFactor = 1.2f;

	pitchSensitivity = 0.001;
	yawSensitivity   = 0.001;

	ground = false;

	// To begin with, we aren't holding down any keys
	holdingForward     = false;
	holdingBackward    = false;
	holdingLeftStrafe  = false;
	holdingRightStrafe = false;

}

void Camera::handleMouseMove(int mouseX, int mouseY){

    glm::vec3 temp;

	// Calculate our horizontal and vertical mouse movement from middle of the window
	float horizMovement = mouseX * yawSensitivity;
	float vertMovement  = mouseY * pitchSensitivity;

	// Apply the mouse movement to vector indicating our view direction
	rotHorizontal -= horizMovement;
	if(rotHorizontal <= 2.0f*PI) rotHorizontal += 2.0f*PI;
	if(rotHorizontal >= 2.0f*PI) rotHorizontal -= 2.0f*PI;
	origin = glm::rotateY(origin-head,-1*horizMovement);
	origin = origin + head;

	front = glm::rotateY(front,-1*horizMovement);
	right = glm::rotateY(right,-1*horizMovement);
	up    = glm::rotateY(right,-1*horizMovement);

    temp = glm::rotate(origin-head,-1*vertMovement,right);

    if(fabs(glm::dot(glm::normalize(temp),glm::vec3(0.0f,1.0f,0.0f))) < 0.8f){

        origin = temp + head;
        rotVertical -= vertMovement;
        if(rotVertical <= 2.0f*PI) rotVertical += 2.0f*PI;
        if(rotVertical >= 2.0f*PI) rotVertical -= 2.0f*PI;

    }

	up = glm::rotate(up,-1*vertMovement,right);
}

// Function to calculate which direction we need to move the camera and by what amount
void Camera::move(double deltatime, Camera* player){

	movement = glm::vec3(0.0f,0.0f,0.0f);

	// Vector that resolves an invalid attempted movement
	glm::vec4 allowed(0.0f,0.0f,0.0f,0.0f);
	glm::vec4 temp(0.0f,0.0f,0.0f,0.0f);

	if (holdingForward)
	{
		movement.x += front.x;
		movement.z += front.z;
	}

	if (holdingBackward)
	{
		movement.x += -1*front.x;
		movement.z += -1*front.z;
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

void Camera::render(Shader& levelShader){

    glClear(GL_DEPTH_BUFFER_BIT);

    glm::mat4 trans = glm::translate(glm::mat4(1.0f), position + 0.1f * front + 0.2f * right);
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotHorizontal + 0.05f, glm::vec3(0.0f, 1.0f, 0.0f));
    rot = glm::rotate(glm::mat4(1.0f), rotVertical, right) * rot;
    currentWeapon->setTransformations(trans,rot);

    currentWeapon->render(levelShader);

}
