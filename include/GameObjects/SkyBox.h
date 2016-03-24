#ifndef SKYBOX_H
#define SKYBOX_H

#include "GameObjects/GameObject.h"
#include "GameObjects/Camera.h"

class SkyBox : public GameObject{

    public:

        SkyBox();
        SkyBox(ObjectData* objectData, Camera* player, GameState* state);
        ~SkyBox();
        SkyBox(const SkyBox & rhs);

        void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);

        void commitMovement(GameState* state){

                translation = glm::translate(glm::mat4(1.0f),player->getPosition());

        }

        void render(GameState* state);

    private:

        Camera* player;

};

#endif
