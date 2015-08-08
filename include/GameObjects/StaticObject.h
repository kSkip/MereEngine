#ifndef STATICOBJECT_H
#define STATICOBJECT_H

#include "GameObject.h"

class StaticObject : public GameObject{

    public:

        StaticObject();
        StaticObject(std::string objectName, float position[3], float rotY, GameState* state);
        ~StaticObject();
        StaticObject(const StaticObject & rhs);

        void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);

        void render(GameState* state);

};

#endif
