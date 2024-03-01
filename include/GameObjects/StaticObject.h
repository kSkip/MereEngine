#ifndef STATICOBJECT_H
#define STATICOBJECT_H

#include "GameObject.h"

class StaticObject : public GameObject{

    public:

        StaticObject();
        StaticObject(ObjectData* objectData, DataBlock& def);
        ~StaticObject();
        StaticObject(const StaticObject & rhs);

        void move(double, Player&);

        void render(Shader&);

};

#endif
