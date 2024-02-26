#include "GameObjects/ParticleSource.h"

ParticleSource::ParticleSource(){}

ParticleSource::ParticleSource(ParticleSourceParams& params_in){

    if(params_in.options & PS_OPT_POINTSPRITE){

        initPointParticles(params_in);

    }else if(params_in.options & PS_OPT_SPRITE){

        initParticles(params_in);

    }

    params = params_in;
    destroy = false;

    collisionTypeValue = NONE;

}

ParticleSource::~ParticleSource(){}

ParticleSource::ParticleSource(const ParticleSource & rhs){}

void ParticleSource::initPointParticles(ParticleSourceParams& params_in){

    unsigned int i;

    srand (time(NULL));
    pointParticles.resize(params_in.numParticles);
    pointVelocities.resize(params_in.numParticles);
    for(i=0;i<params_in.numParticles;i++){

        pointParticles[i].position.x = params_in.position.x;
        pointParticles[i].position.y = params_in.position.y;
        pointParticles[i].position.z = params_in.position.x;

        pointVelocities[i].x = (float(rand()) / float(RAND_MAX) - 0.5f)*params_in.maxInitVelocity.x;
        pointVelocities[i].y = (float(rand()) / float(RAND_MAX) - 0.5f)*params_in.maxInitVelocity.y;
        pointVelocities[i].z = (float(rand()) / float(RAND_MAX) - 0.5f)*params_in.maxInitVelocity.z;

        pointParticles[i].color.r = 0.0f;
        pointParticles[i].color.g = 0.0f;
        pointParticles[i].color.b = 0.0f;

    }

}

void ParticleSource::initParticles(ParticleSourceParams& params_in)
{

}

void ParticleSource::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){

    unsigned int i;

    if(params.options & PS_OPT_POINTSPRITE){

        for(i=0;i<pointParticles.size();i++){

            pointParticles[i].position.x += float(deltatime)*pointVelocities[i].x;
            pointParticles[i].position.y += float(deltatime)*pointVelocities[i].y;
            pointParticles[i].position.z += float(deltatime)*pointVelocities[i].z;

            //simulate gravity
            pointVelocities[i].y -= GRAVITATIONAL_ACCELERATION*float(deltatime);

            //decrease the life of the particle
            life -= deltatime;

        }

    }

}

void ParticleSource::commitMovement()
{

}

void ParticleSource::render(){

    if(params.options & PS_OPT_POINTSPRITE){

        //renderMeshArrays();

    }

}
