#ifndef STATICOBJECT_H
#define STATICOBJECT_H

#include "GameObject.h"
#include "Models/OBJMesh.h"

class StaticObject : public GameObject {
public:
    StaticObject(OBJMesh*);
    ~StaticObject();

    void draw(Shader&);
    boundary* getBounds() { return &model->bounds; }

private:
    OBJMesh* model;
};

#endif