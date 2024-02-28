#ifndef PARTICLESOURCE_H
#define PARTICLESOURCE_H

#include "GameObjects/GameObject.h"

struct Particle {
    float position[3];
    float velocity[3];
};

#define PS_OPT_POINTSPRITE 1
#define PS_OPT_SPRITE 2

struct ParticleSourceParams{

    std::string objectName;

    vec3 position;
    vec3 maxInitVelocity;
    unsigned int numParticles;
    float lifetime;
    bool continuous;
    float generationRate;
    bool gravity;

    vec3 color;

    unsigned short options;

};

class ParticleSource : public GameObject{

    public:

        ParticleSource();
        ParticleSource(ParticleSourceParams& params);
        ~ParticleSource();
        ParticleSource(const ParticleSource & rhs);


        void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);

        void commitMovement();

        void render();

    private:

        void initPointParticles(ParticleSourceParams& params);
        void initParticles(ParticleSourceParams& params);

        std::vector<vertex> pointParticles;
        std::vector<vec3> pointVelocities;

        std::list<Particle*> particles;

        ParticleSourceParams params;

};

#endif
