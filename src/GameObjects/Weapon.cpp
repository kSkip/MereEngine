#include "GameObjects/Weapon.h"

Weapon::Weapon() :
    animTime(0.0),
    isFiring(0),
    data(nullptr),
    rotY(0.0f)
{
    skipMVP = false;
    skipLighting = false;
}

void Weapon::setTransformations(mat4& translation_in, mat4& rotation_in){

    translation = translation_in;
    rotation = rotation_in;

}

void Weapon::move(double deltatime)
{    
    if(data){
        if (Armature* arm = data->armatures["fire"]) {
            if (isFiring) {
                animTime += deltatime;
                if (animTime > arm->getTotalTime()) {
                    animTime = 0.0;
                    isFiring = 0;
                }
            }
            else {
                animTime = 0.0;
            }
            arm->buildFrame(animTime);
            arm->setVertices(data->vertices.data(), data->unskinnedVertices.data(), data->numVertices);
        }
    }

}

void Weapon::render(Shader& levelShader){

    if(data){

        glBindBuffer(GL_ARRAY_BUFFER, data->vertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER,0,data->numVertices*sizeof(struct vertex),data->vertices.data());

        glUniformMatrix4fv(levelShader.translation, 1, GL_FALSE, (GLfloat*)&this->translation);
        glUniformMatrix4fv(levelShader.rotation, 1, GL_FALSE, (GLfloat*)&this->rotation);

        glUniform1ui(levelShader.skipMVP, this->skipMVP);
        glUniform1ui(levelShader.skipLighting, this->skipLighting);

        glBindTexture(GL_TEXTURE_2D, data->diffuseTex);

        glBindBuffer(GL_ARRAY_BUFFER, data->vertexBuffer);

        glVertexAttribPointer(
            LOCATION_POSITION,
            3, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
            (void*)offsetof(struct vertex, position)
        );
        glVertexAttribPointer(
            LOCATION_NORMAL,
            3, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
            (void*)offsetof(struct vertex, normal)
        );
        glVertexAttribPointer(
            LOCATION_TEXCOORD,
            2, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
            (void*)offsetof(struct vertex, texcoord)
        );

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->elementBuffer);

        glDrawElements(
            GL_TRIANGLES,
            data->elementCount,
            GL_UNSIGNED_INT,
            (void*)0
        );

    }

}
