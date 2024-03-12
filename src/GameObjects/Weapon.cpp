#include "GameObjects/Weapon.h"
#include "Models/MD5Model.h"

Weapon::Weapon() :
    animTime(0.0),
    isFiring(0),
    rotY(0.0f),
    model(nullptr)
{
    skipMVP = false;
    skipLighting = false;

    translation = mat4(1.0f);
    rotation = mat4(1.0f);

    float yaw = -5.0f;
    float pitch = 185.0f;
    float cosYaw = cos(toRadians(yaw));
    float sinYaw = sin(toRadians(yaw));
    float cosPitch = cos(toRadians(pitch));
    float sinPitch = sin(toRadians(pitch));

    rotation[0] = vec4(cosPitch, 0.0f, -sinPitch, 0.0f);
    rotation[1] = vec4(sinYaw * sinPitch, cosYaw, sinYaw * cosPitch, 0.0f);
    rotation[2] = vec4(cosYaw * sinPitch, -sinYaw, cosPitch * cosYaw, 0.0f);
    translation[3] = vec4(0.2f, -0.3f, 0.0f, 1.0f);
}

void Weapon::move(double deltatime)
{
    if(model) {
        if (isFiring) {
            animTime += deltatime;
            if (animTime > model->animation.getTotalTime()) {
                animTime = 0.0;
                isFiring = 0;
            }
        }
        else {
            animTime = 0.0;
        }
        model->setAnimation(animTime);
    }
}

void Weapon::draw(Shader& shader)
{
    model->draw(shader, translation, rotation, skipMVP, skipLighting);
}