#ifndef SKYBOX_H
#define SKYBOX_H

#include "GameObjects/GameObject.h"
#include "GameObjects/Camera.h"

class SkyBox : public GameObject{

    public:

        SkyBox();
        SkyBox(ObjectData* objectData, Camera* player);
        ~SkyBox();
        SkyBox(const SkyBox & rhs);

        void move(double deltatime, Camera* player);

        void commitMovement();

        void render(Shader&);

    private:

        Camera* player;

};

#endif
