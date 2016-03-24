#include "GameObjects/Camera.h"
#include "Utilities/TextToTexture.h"
#include "Utilities/TextManipulation.h"
#include "Utilities/DataBlock.h"

#include <SDL/SDL.h>

Camera::Camera(ObjectData* objectData, float windowSize[2], DataBlock& def, GameState* state){

	initCamera();

    strvec pos = split(def("position"),',');

    if(pos.size() != 4)
        throw std::runtime_error("Position requires 4 values");

    float pos_init[3];
    pos_init[0] = atof(pos[0].c_str());
    pos_init[1] = atof(pos[1].c_str());
    pos_init[2] = atof(pos[2].c_str());

    rotY  = atof(pos[3].c_str());

	position = glm::vec3(pos_init[0],pos_init[1],pos_init[2]);
	head     = position + glm::vec3(0.0f,0.3f,0.0f);
	origin   = head + glm::vec3(0.0f,0.0f,1000.0f);

	origin = glm::rotateY(origin-head,-1*rotY);
	origin = origin + head;

	front = glm::rotateY(front,-1*rotY);
	right = glm::rotateY(right,-1*rotY);

	data = objectData;

    float crossPos[2];
    crossPos[0] = 0.0f;
    crossPos[1] = 0.0f;
    float crossScale[2];
    crossScale[0] = 0.05f * windowSize[1] / windowSize[0];
    crossScale[1] = 0.05f;

    std::string crossTexFile = def("rootdir") + def("crosshairs");

    crosshairs = new Billboard(crossTexFile.c_str(), crossPos, crossScale, state);

    nameCurrentWeapon = "gun";
    currentWeapon = new Weapon("gun",state);

    std::string healthFontFile = def("rootdir") + def("healthfont");

    GLuint healthMeterTexture;
    SDL_Color text_color = {0, 127, 0};
    healthMeterFont = TTF_OpenFont(healthFontFile.c_str(), 32);
    glGenTextures(1, &healthMeterTexture);
    glBindTexture(GL_TEXTURE_2D, healthMeterTexture);
    TextToTexture("100",healthMeterFont,text_color);

    float healthMeterPos[2] = {-0.8f,-0.8f};
    float healthMeterScale[2];
    healthMeterScale[0] = 0.1f * 1.75f * (windowSize[1] / windowSize[0]);
    healthMeterScale[1] = 0.1f;
    healthMeter = new Billboard(healthMeterTexture,healthMeterPos,healthMeterScale,state);

    destroy = false;

    skipMVP = true;
    skipLighting = true;

    cDistance = 0.0f;

    collisionTypeValue = COLLIDER_COLLIDEE;

	windowWidth  = windowSize[0];
	windowHeight = windowSize[1];

	windowMidX = windowWidth  / 2.0f;
	windowMidY = windowHeight / 2.0f;

	SDL_WarpMouse(windowMidX,windowMidY);

}

Camera::~Camera(){}

void Camera::initCamera(){

	position = glm::vec3(0.0f,0.0f,0.0f);
	head = glm::vec3(0.0f,0.0f,0.0f);
	origin = glm::vec3(0.0f,0.0f,0.0f);
	front = glm::vec3(0.0f,0.0f,1.0f);
	right = glm::vec3(-1.0f,0.0f,0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	velocity = glm::vec3(0.0f,0.0f,0.0f);

    rotHorizontal = 0.0f;
    rotVertical = 0.0f;

	movementSpeedFactor = 1.2f;

	pitchSensitivity = 0.1;
	yawSensitivity   = 0.1;

	ground = false;

	// To begin with, we aren't holding down any keys
	holdingForward     = false;
	holdingBackward    = false;
	holdingLeftStrafe  = false;
	holdingRightStrafe = false;

	playerIsFiring = false;

}

void Camera::handleMouseMove(int mouseX, int mouseY){

    glm::vec3 temp;

	// Calculate our horizontal and vertical mouse movement from middle of the window
	float horizMovement = (mouseX - windowMidX) * yawSensitivity;
	float vertMovement  = (mouseY - windowMidY) * pitchSensitivity;

	// Apply the mouse movement to vector indicating our view direction
	rotHorizontal -= horizMovement;
	if(rotHorizontal <= 360.0f) rotHorizontal += 360.0f;
	if(rotHorizontal >= 360.0f) rotHorizontal -= 360.0f;
	origin = glm::rotateY(origin-head,-1*horizMovement);
	origin = origin + head;

	front = glm::rotateY(front,-1*horizMovement);
	right = glm::rotateY(right,-1*horizMovement);
	up = glm::rotateY(right,-1*horizMovement);

    temp = glm::rotate(origin-head,-1*vertMovement,right);

    if(fabs(glm::dot(glm::normalize(temp),glm::vec3(0.0f,1.0f,0.0f))) < 0.8f){

        origin = temp + head;
        rotVertical -= vertMovement;
        if(rotVertical <= 360.0f) rotVertical += 360.0f;
        if(rotVertical >= 360.0f) rotVertical -= 360.0f;

    }

	up = glm::rotate(up,-1*vertMovement,right);

	SDL_WarpMouse(windowMidX,windowMidY);

}

// Function to calculate which direction we need to move the camera and by what amount
void Camera::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){

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


}

void Camera::fire(){

    playerIsFiring = true;
    //flashLife = 1.0f;

}

void Camera::render(GameState* state){

    crosshairs->render(state);

    healthMeter->render(state);

    glClear(GL_DEPTH_BUFFER_BIT);

    glm::mat4 trans = glm::translate(glm::mat4(1.0f),position + 0.2f*right);
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f),rotVertical,right) * glm::rotate(glm::mat4(1.0f),rotHorizontal+2.0f,glm::vec3(0.0f,1.0f,0.0f));
    currentWeapon->setTransformations(trans,rot);

    currentWeapon->render(state);

}
