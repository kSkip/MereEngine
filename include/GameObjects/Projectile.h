#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameObject.h"

class Projectile : public GameObject{

    public:

        Projectile();
        Projectile(std::string objectName, glm::vec3 position, glm::vec3 speed, glm::vec3 align, float roty, GameState* state);
        ~Projectile();
        Projectile(const Projectile & rhs);

        void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);

        void commitMovement(GameState* state){

                position += movement; translation = glm::translate(glm::mat4(1.0f),position);

        }

        void render(GameState* state);

};

#endif
