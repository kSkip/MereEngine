#ifndef PARTICLE_H
#define PARTILCE_H

#include "GameObjects/GameObject.h"

struct ParticleParams{

    std::string objectName;

    glm::vec3 initPosition;
    glm::vec3 initVelocity;
    float life;
    bool gravity;

};

class Particle : public GameObject{

    public:

        Particle();
        Particle(ParticleParams& params, GameState* state);
        ~Particle();
        Particle(const Particle & rhs);

        void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);

        void commitMovement(GameState* state);

        void render(GameState* state);

    private:

        bool gravity;

};

#endif
