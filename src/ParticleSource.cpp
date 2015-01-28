#include "GameObject.h"

ParticleSource::ParticleSource(){}

ParticleSource::ParticleSource(const char* objectfile, glm::vec3 p, glm::vec3 maxv, float numParticles, float lifetime, bool continuous, int generationRate, bool gravity, GameState* state){
    unsigned int i;

    Assimp::Importer importer;
    if(objectMap.find(std::string(objectfile)) == objectMap.end()){

        Assimp::Importer* importer = new Assimp::Importer();
        boundary* objectBounds = new boundary;
        ObjectData* newObjectData = new ObjectData;

        const aiScene* cpScene = importer->ReadFile(objectfile,aiProcess_GenSmoothNormals);

        aiMatrix4x4 m_GlobalInverseTransform = cpScene->mRootNode->mTransformation;
        m_GlobalInverseTransform.Inverse();
        float temp[16]={m_GlobalInverseTransform.a1,m_GlobalInverseTransform.a2,m_GlobalInverseTransform.a3,m_GlobalInverseTransform.a4,
                        m_GlobalInverseTransform.b1,m_GlobalInverseTransform.b2,m_GlobalInverseTransform.b3,m_GlobalInverseTransform.b4,
                        m_GlobalInverseTransform.c1,m_GlobalInverseTransform.c2,m_GlobalInverseTransform.c3,m_GlobalInverseTransform.c4,
                        m_GlobalInverseTransform.d1,m_GlobalInverseTransform.d2,m_GlobalInverseTransform.d3,m_GlobalInverseTransform.d4};
        newObjectData->globalInverseTransform = glm::make_mat4(temp);
        newObjectData->globalInverseTransform = glm::transpose(newObjectData->globalInverseTransform);

        newObjectData->VertexBuffer = makeArrayBuffer(cpScene->mMeshes[0],newObjectData);
        newObjectData->ElementBuffer = makeElementBuffer(cpScene->mMeshes[0]);
        newObjectData->ElementCount = cpScene->mMeshes[0]->mNumFaces * 3;
        newObjectData->TextureId = makeGetTexBuf(cpScene->mMaterials[0],state);
        newObjectData->Importer = importer;
        newObjectData->objectBounds = objectBounds;

        objectMap[std::string(objectfile)] = newObjectData;
        data = newObjectData;
    }else{
        data = objectMap[std::string(objectfile)];
    }

    srand (time(NULL));
    for(i=0;i<numParticles;i++){
        Particle* newParticle = new Particle(objectfile, p, glm::vec3( (float(rand()) / float(RAND_MAX) - 0.5f)*maxv.x, (float(rand()) / float(RAND_MAX) - 0.5f)*maxv.y, (float(rand()) / float(RAND_MAX) - 0.5f) )*maxv.z, lifetime, gravity, state);
        addTransparentObject(newParticle,state);
        particles.push_back(newParticle);
    }

    position = p;
    destroy = false;

    collisionTypeValue = NONE;
}

ParticleSource::~ParticleSource(){}

ParticleSource::ParticleSource(const ParticleSource & rhs){}

void ParticleSource::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){
}

void ParticleSource::commitMovement(GameState* state){
    std::list<Particle*>::iterator it;

    for(it = particles.begin();it != particles.end();){
        if((*it)->getLife() < 0.0f){
            (*it)->makeDestroy();
            it = particles.erase(it);
        }else{
			it++;
		}

    }

    if(particles.size() == 0) destroy = true;
}

void ParticleSource::render(GameState* state){
}
