#include "GameObjects/ParticleSource.h"

ParticleSource::ParticleSource(){}

ParticleSource::ParticleSource(ParticleSourceParams& params_in, GameState* state){

    if(params_in.options & PS_OPT_POINTSPRITE){

        initPointParticles(params_in,state);

    }else if(params_in.options & PS_OPT_SPRITE){

        initParticles(params_in,state);

    }

    params = params_in;
    destroy = false;

    collisionTypeValue = NONE;

}

ParticleSource::~ParticleSource(){}

ParticleSource::ParticleSource(const ParticleSource & rhs){}

void ParticleSource::initPointParticles(ParticleSourceParams& params_in, GameState* state){

    unsigned int i;

    srand (time(NULL));
    pointParticles.resize(params_in.numParticles);
    pointVelocities.resize(params_in.numParticles);
    for(i=0;i<params_in.numParticles;i++){

        pointParticles[i].position[0] = params_in.position.x;
        pointParticles[i].position[1] = params_in.position.y;
        pointParticles[i].position[2] = params_in.position.x;

        pointVelocities[i].x = (float(rand()) / float(RAND_MAX) - 0.5f)*params_in.maxInitVelocity.x;
        pointVelocities[i].y = (float(rand()) / float(RAND_MAX) - 0.5f)*params_in.maxInitVelocity.y;
        pointVelocities[i].z = (float(rand()) / float(RAND_MAX) - 0.5f)*params_in.maxInitVelocity.z;

        pointParticles[i].color[0] = 0.0f;
        pointParticles[i].color[1] = 0.0f;
        pointParticles[i].color[2] = 0.0f;

    }

}

void ParticleSource::initParticles(ParticleSourceParams& params_in, GameState* state)
{

}

void ParticleSource::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){

    unsigned int i;

    if(params.options & PS_OPT_POINTSPRITE){

        for(i=0;i<pointParticles.size();i++){

            pointParticles[i].position[0] += float(deltatime)*pointVelocities[i].x;
            pointParticles[i].position[1] += float(deltatime)*pointVelocities[i].y;
            pointParticles[i].position[2] += float(deltatime)*pointVelocities[i].z;

            //simulate gravity
            pointVelocities[i].y -= GRAVITATIONAL_ACCELERATION*float(deltatime);

            //decrease the life of the particle
            life -= deltatime;

        }

    }

}

void ParticleSource::commitMovement(GameState* state)
{

}

void ParticleSource::render(GameState* state){

    if(params.options & PS_OPT_POINTSPRITE){

        //renderMeshArrays();

    }

}
