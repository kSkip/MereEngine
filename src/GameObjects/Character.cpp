#include "GameObjects/Character.h"

Character::Character(const vec3& pos, Player& p, MD5Model* model) : player(p), model(model)
{
    rotY = 0.0f;
    position = pos;
    translation = mat4(1.0f);
    translation[3].x = position.x;
    translation[3].y = position.y;
    translation[3].z = position.z;

    float sinRotY = sin(rotY);
    float cosRotY = cos(rotY);

    rotation = mat4(1.0f);
    rotation[0][0] = cosRotY;
    rotation[0][2] = -sinRotY;
    rotation[2][0] = sinRotY;
    rotation[2][2] = cosRotY;
    front = vec3(sinRotY, 0.0f, cosRotY);
    right = vec3(cosRotY, 0.0f, -sinRotY);

    velocity = vec3(0.0f, 0.0f, 0.0f);

    movementSpeedFactor = 0.5f;

    ground              = false;
    health              = 5.0f;
    destroy             = false;
    skipMVP             = false;
    skipLighting        = false;

    collisionTypeValue = COLLIDER_COLLIDEE;
    charAnimTime = 0;
}

Character::~Character(){}

void Character::trackPlayer(double deltatime)
{

    vec4 allowed, temp;

    movement = vec3(0.0f, 0.0f, 0.0f);
    allowed = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    temp = vec4(0.0f, 0.0f, 0.0f, 0.0f);

    currentTarget = player.getPosition();

    vec3 vectorToTarget = currentTarget - position;
    float length = sqrt(dot(vectorToTarget, vectorToTarget));
    vectorToTarget = vec3(vectorToTarget.x, 0.0f, vectorToTarget.z);
    float frontDotProduct = dot(vectorToTarget, front) / length;
    float rightDotProduct = dot(vectorToTarget, right) / length;

    frontDotProduct = min(frontDotProduct, 1.0f);
    frontDotProduct = max(frontDotProduct, -1.0f);

    rightDotProduct = min(rightDotProduct, 1.0f);
    rightDotProduct = max(rightDotProduct, -1.0f);

    float deltaRot = 0.1f * acos(frontDotProduct) / (length + 1);
    float twoPI = 2.0 * PI;
    if (rightDotProduct > 0.0) {
        rotY += deltaRot;
        if (rotY <= 0.0) {
            rotY += twoPI;
        }
    }
    else if (rightDotProduct < 0.0) {
        rotY -= deltaRot;
        if (rotY >= twoPI) {
            rotY -= twoPI;
        }
    }
}

void Character::move(double deltatime)
{

    charAnimTime += deltatime;

    trackPlayer(deltatime);

    float sinRotY = sin(rotY);
    float cosRotY = cos(rotY);

    rotation = mat4(1.0f);
    rotation[0][0] = cosRotY;
    rotation[0][2] = -sinRotY;
    rotation[2][0] = sinRotY;
    rotation[2][2] = cosRotY;
    front = vec3(sinRotY, 0.0f, cosRotY);
    right = vec3(cosRotY, 0.0f, -sinRotY);

    velocity.y -= GRAVITATIONAL_ACCELERATION * deltatime;

    movement.x = movementSpeedFactor * deltatime * front.x;
    movement.y = deltatime * velocity.y;
    movement.z = movementSpeedFactor * deltatime * front.z;
}

void Character::commitMovement() {

    position += movement;
    translation = mat4(1.0f);
    translation[3].x = position.x;
    translation[3].y = position.y;
    translation[3].z = position.z;
    if (health < 0.0f)destroy = true;

}

void Character::damage(float magnitude, vec3 damageLocation)
{
    health -= magnitude;
}

void Character::draw(Shader& shader)
{
    model->setAnimation(charAnimTime);
    model->draw(shader, translation, rotation, skipMVP, skipLighting);
}
