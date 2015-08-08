#ifndef PARTICLESOURCE_H
#define PARTICLESOURCE_H

#include "GameObjects/GameObject.h"
#include "GameObjects/Particle.h"

#define PS_OPT_POINTSPRITE 1
#define PS_OPT_SPRITE 2

struct ParticleSourceParams{

    std::string objectName;

    glm::vec3 position;
    glm::vec3 maxInitVelocity;
    unsigned int numParticles;
    float lifetime;
    bool continuous;
    float generationRate;
    bool gravity;

    glm::vec3 color;

    unsigned short options;

};

class ParticleSource : public GameObject{

    public:

        ParticleSource();
        ParticleSource(ParticleSourceParams& params, GameState* state);
        ~ParticleSource();
        ParticleSource(const ParticleSource & rhs);


        void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);

        void commitMovement(GameState* state);

        void render(GameState* state);

    private:

        void initPointParticles(ParticleSourceParams& params, GameState* state);
        void initParticles(ParticleSourceParams& params, GameState* state);

        std::vector<vertex> pointParticles;
        std::vector<glm::vec3> pointVelocities;

        std::list<Particle*> particles;

        ParticleSourceParams params;

};

#endif
