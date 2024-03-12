#ifndef SKYBOX_H
#define SKYBOX_H

#include "GameObjects/GameObject.h"
#include "Player.h"
#include "Models/OBJMesh.h"

class SkyBox : public GameObject {
public:
    SkyBox(Player&, OBJMesh*);
    void commitMovement();

    void draw(Shader&);
    boundary* getBounds() { return &model->bounds; }

private:
    Player& player;
    OBJMesh* model;
};

#endif
