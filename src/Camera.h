#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <SDL/SDL.h>

#include "GameObject.h"
#include "boundary.h"

class GameObject;

class Camera
{
    protected:
        // Camera position
        glm::vec3 position;

        // For glm::lookAt
        glm::vec3 origin;

        // Camera rotation
        double rotation;

        // Account for the orientation
        glm::vec3 front;
        glm::vec3 right;

        // Camera movement speed. When we call the move() function on a camera, it moves using these speeds
        glm::vec3 velocity;

	bool ground;

        double movementSpeedFactor; // Controls how fast the camera moves
        double pitchSensitivity;    // Controls how sensitive mouse movements affect looking up and down
        double yawSensitivity;      // Controls how sensitive mouse movements affect looking left and right

        // Window size in pixels and where the midpoint of it falls
        int windowWidth;
        int windowHeight;
        int windowMidX;
        int windowMidY;

        // Method to set some reasonable default values. For internal use by the class only.
        void initCamera();

    public:
        static const double TO_RADS; // The value of 1 degree in radians

        // Holding any keys down?
        bool holdingForward;
        bool holdingBackward;
        bool holdingLeftStrafe;
        bool holdingRightStrafe;

        // Constructors
        Camera(float windowWidth, float windowHeight);

        // Destructor
        ~Camera();

        // Mouse movement handler to look around
        void handleMouseMove(int mouseX, int mouseY);

        // Method to convert an angle in degress to radians
        const double toRads(const double &angleInDegrees) const;

        // Method to move the camera based on the current direction
        void move(double deltaTime, std::vector<GameObject*>* levelObjects);

	bool onGround() const {return ground;}

        // --------------------------------- Inline methods ----------------------------------------------

        // Setters to allow for change of vertical (pitch) and horizontal (yaw) mouse movement sensitivity
        float getPitchSensitivity()            { return pitchSensitivity;  }
        void  setPitchSensitivity(float value) { pitchSensitivity = value; }
        float getYawSensitivity()              { return yawSensitivity;    }
        void  setYawSensitivity(float value)   { yawSensitivity   = value; }

        // Position getters and setters
        glm::vec3 getPosition() const { return position;        }
        double getXPos()           const { return position.x; }
        double getYPos()           const { return position.y; }
        double getZPos()           const { return position.z; }

	void setXPos(double x)	{position.x = x;}
	void setYPos(double y)	{position.y = y;}
	void setZPos(double z)	{position.z = z;}

	// Speed getters and setters
        glm::vec3 getVelocity() const { return velocity;        }
        double getXSpeed()           const { return velocity.x; }
        double getYSpeed()           const { return velocity.y; }
        double getZSpeed()           const { return velocity.z; }

	void setXSpeed(double x)	{velocity.x = x;}
	void setYSpeed(double y)	{ground = false; velocity.y = y;}
	void setZSpeed(double z)	{velocity.z = z;}

	// Origin getters and setters
        glm::vec3 getOrigin() const { return origin;        }
        double getXOrigin()           const { return origin.x; }
        double getYOrigin()           const { return origin.y; }
        double getZOrigin()           const { return origin.z; }

	void setXOrigin(double x)	{origin.x = x;}
	void setYOrigin(double y)	{origin.y = y;}
	void setZOrigin(double z)	{origin.z = z;}

	// Front getters and setters
        glm::vec3 getFront() const { return front;        }
        double getXFront()           const { return front.x; }
        double getYFront()           const { return front.y; }
        double getZFront()           const { return front.z; }

	void setXFront(double x)	{front.x = x;}
	void setYFront(double y)	{front.y = y;}
	void setZFront(double z)	{front.z = z;}

	// Right getters and setters
        glm::vec3 getRight() const { return right;        }
        double getXRight()           const { return right.x; }
        double getYRight()           const { return right.y; }
        double getZRight()           const { return right.z; }

	void setXRight(double x)	{right.x = x;}
	void setYRight(double y)	{right.y = y;}
	void setZRight(double z)	{right.z = z;}

};

#endif // CAMERA_H
