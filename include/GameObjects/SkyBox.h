#ifndef SKYBOX_H
#define SKYBOX_H

#include "GameObjects/GameObject.h"
#include "Player.h"

class SkyBox : public GameObject{
public:
    SkyBox(ObjectData*, Player&);
    void commitMovement();
    void render(Shader&);

private:
    Player& player;

};

#endif
