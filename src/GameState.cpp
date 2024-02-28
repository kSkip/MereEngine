#include <numeric>

#include "GameState.h"
#include "GameObjects/StaticObject.h"
#include "GameObjects/Character.h"
#include "GameObjects/SkyBox.h"
#include "GameObjects/Bullet.h"
#include "Utilities/TextManipulation.h"
#include "VectorMath.h"

struct FrameRateCounter {
    std::vector<double> samples;
    size_t idx;

    FrameRateCounter() : idx(0), samples(16) {}

    void insert(double frameTime) {
        samples[idx] = frameTime;
        idx = (idx + 1) % samples.size();
    }

    double getMillis() {
        double result = std::accumulate(samples.begin(), samples.end(), 0.0);
        return  result / samples.size();
    }
} counter;

GameState::GameState(){}

GameState::~GameState()
{

    std::list<GameObject*>::iterator i;

    //cleanup everything before exiting program
    delete levelShader;

    for(i = levelObjects.begin(); i != levelObjects.end(); i++)
        delete (*i);

    levelObjects.clear();

    opaqueObjects.clear();
    transparencyObjects.clear();

}

GameState::GameState(const GameState & rhs){}

void GameState::init(std::string dir)
{

    rootDir = dir;

    /*
     * Indicate no state has been loaded yet
     */

    cursorVisible = false;
    ShowCursor(FALSE);
	loaded = false;
    paused = false;

}

void GameState::clean()
{

    std::list<GameObject*>::iterator i;
    std::map<std::string,GLuint>::iterator textureIt;

    delete levelShader;

    for(i = levelObjects.begin(); i != levelObjects.end(); i++)
        delete (*i);

    levelObjects.clear();

    opaqueObjects.clear();
    transparencyObjects.clear();

    std::map<std::string,ObjectData*>::iterator objIt;

    for(objIt = objectMap.begin(); objIt != objectMap.end(); objIt++){

        glDeleteBuffers(1,&objIt->second->vertexBuffer);
        glDeleteBuffers(1,&objIt->second->elementBuffer);
        glDeleteTextures(1,&objIt->second->diffuseTex);
        delete objIt->second->objectBounds;

    }

    objectMap.clear();

    loaded = false;

}

void GameState::setWindowSize(int width, int height)
{
    context.width = width;
    context.height = height;

    screenwidth = width;
    screenheight = height;
    aspectRatio = float(screenwidth) / float(screenheight);
}

void GameState::loadObjectData(DataBlock & objectDataBlock)
{

    block_it it = objectDataBlock.beginBlocks();

    while(it != objectDataBlock.endBlocks())
    {

        std::string name      = it->first;
        DataBlock& objectData = it->second;

        objectData("rootdir") = rootDir;

        objectMap[name] = new ObjectData(objectData);

        ++it;

    }

}

void GameState::loadObjects(DataBlock & objectBlock)
{

    block_it it = objectBlock.beginBlocks();

    while(it != objectBlock.endBlocks())
    {

        DataBlock& objectData = it->second;

        ObjectData* object = objectMap[it->first];

        std::string objectType = objectData("obj_type");

        objectData("rootdir") = rootDir;

        if(objectType == "camera"){

            float windowSize[2];
            windowSize[0] = screenwidth;
            windowSize[1] = screenheight;

            player = new Camera(object, objectMap["gun"]);

            insertOpaqueObject(player);

        }else if(objectType == "static_object"){

            StaticObject* newGameObject = new StaticObject(object, objectData);

            insertOpaqueObject(newGameObject);

        }else if(objectType == "character"){

            Character* newGameObject = new Character(object, objectData);

            insertOpaqueObject(newGameObject);

        }else if(objectType == "sky_box"){

            SkyBox* newGameObject = new SkyBox(object, player);

            insertOpaqueObject(newGameObject);

        }

        ++it;

    }

}

void GameState::loadNew(std::string levelfile)
{
    level = levelfile;

    DataBlock leveldef;

    leveldef.load(levelfile.c_str());

    try{

        /*
         * load shaders
         */
        std::string vs = rootDir + leveldef["shaders"]("vs");

        std::string fs = rootDir + leveldef["shaders"]("fs");

        levelShader = new Shader;

        levelShader->loadShader(vs.c_str(),fs.c_str());

    }
    catch(std::exception& e)
    {

        std::cerr << e.what() << "\n";
        delete levelShader;
        throw std::runtime_error("Failed to init shaders");

    }

    try{

        /*
         * load shared data and GPU buffers
         */
        loadObjectData(leveldef["objectdata"]);

    }
    catch(std::exception& e)
    {

        std::cerr << e.what() << "\n";
        throw std::runtime_error("Failed to load shared and GPU buffer");

    }

    try{

        /*
         * load object instances
         */
        loadObjects(leveldef["objects"]);

    }
    catch(std::exception& e)
    {

        std::cerr << e.what() << "\n";
        throw std::runtime_error("Failed to load object instances");

    }


    loaded = true;

}

void GameState::loadSave(std::string savefile){}

void GameState::save(){}

void GameState::movingForward(bool isMoving)
{
    player->holdingForward = isMoving;
}

void GameState::movingBackward(bool isMoving)
{
    player->holdingBackward = isMoving;
}

void GameState::movingLeft(bool isMoving)
{
    player->holdingLeftStrafe = isMoving;
}

void GameState::movingRight(bool isMoving)
{
    player->holdingRightStrafe = isMoving;
}

void GameState::handleSpacebar()
{
    if (player->onGround()) {
        player->setYSpeed(1.5f);
        player->isNotGrounded();
    }
}

void GameState::handleEscape() {
    paused = !paused;
}

void GameState::handleLeftButtonDown() {
    if (paused) {
        context.cursor.leftButtonDown = true;
    }
    else {
        firePrimaryWeapon();
    }
}

void GameState::handleMouseMove(int x, int y)
{
    if (paused) {
        context.cursor.x = x;
        context.cursor.y = y;
    }
}

void GameState::handleRelativeMouseMove(int dx, int dy)
{
    if (!paused) {
        context.cursor.dx += dx;
        context.cursor.dy += dy;
    }
}

void GameState::firePrimaryWeapon()
{
    Weapon & weapon = player->GetWeapon();
    if (!weapon.isFiring) {

        vec3 Begin = player->getHead() + 0.1f * player->getFront();
        vec3 End = player->getHead() + 100.0f * player->getFront();
        GameObject* newobject = new Bullet(Begin, End, player);

        levelObjects.push_back(newobject);
        opaqueObjects.push_back(newobject);

        weapon.fire();
    }
}

bool GameState::run(double elapsedTime)
{
    if (paused) {
        if (!cursorVisible) {
            ShowCursor(TRUE);
            cursorVisible = 1;
        }

        MenuAction action = MenuAction::NOACTION;
        renderMenu(context, action);
        switch (action) {
        case MenuAction::NEWGAME:
            //if (loaded) clean();
            //loadNew(level);
            //paused = 0;
            break;
        case MenuAction::EXIT:
            return true;
        default:
            break;
        }
    }
    else {
        if (cursorVisible) {
            ShowCursor(FALSE);
            cursorVisible = 0;
        }

        move(elapsedTime);
        render();

        glColor3f(0.0f, 0.7f, 0.0f);
        float top = (-(float)screenheight + 24.0f) / (float)screenheight;
        float left = ((float)screenwidth - 12.0f) / (float)screenwidth;
        glRasterPos3f(-left, -top, 1.0f);
        char fps[16];
        counter.insert(1000 * elapsedTime);
        sprintf(fps, "%4.0f", 1000 / counter.getMillis());
        glCallLists(strlen(fps), GL_UNSIGNED_BYTE, fps);
    }
    return false;
}

//moves the game objects through time
void GameState::move(double deltatime)
{
    player->handleMouseMove(context.cursor.dx, context.cursor.dy);
    context.cursor.dx = 0;
    context.cursor.dy = 0;

    std::list<GameObject*>::iterator i, j;

    for(i = levelObjects.begin(); i != levelObjects.end(); i++)
        (*i)->move(deltatime, this->player);


    for(i = levelObjects.begin(); i != levelObjects.end(); i++)
    {
        for(j = i; j != levelObjects.end(); j++)
        {
            if(i != j) GameObject::testResolveCollision((*i),(*j)); //detect and resolve collisions
        }
    }

    for(i = levelObjects.begin(); i != levelObjects.end();)

    {
        (*i)->commitMovement(); //commit to the new changes to the objects

        if((*i)->isDestroyed()){ //deallocate and remove objects that no longer exist

            delete *i;
            opaqueObjects.remove(*i);
            transparencyObjects.remove(*i);
            i = levelObjects.erase(i);

        }else i++;

    }

}

//renders everything
void GameState::render()
{

    std::map<std::string,GLuint>::iterator textureIt;
    std::list<GameObject*>::iterator i;
    mat4 perspectiveMatrix;
    mat4 viewMatrix;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    perspectiveMatrix = perspective(1.04f, aspectRatio, 0.1f, 1000.0f);
    player->getViewMatrix(viewMatrix);

    levelShader->activate(ENABLE_POSITION | ENABLE_NORMAL | ENABLE_TEXCOORD); //enable all attributes

    glUniformMatrix4fv(levelShader->modelView, 1, GL_FALSE, (GLfloat*)&viewMatrix);
    glUniformMatrix4fv(levelShader->projection, 1, GL_FALSE, (GLfloat*)&perspectiveMatrix);


    //Use of the only the first texture only for now
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(levelShader->texture, 0);

    /*
     * Render the opaque objects first t ofill the depth buffer
     * Then the transparent obejcts will go next
     */
    for(i = opaqueObjects.begin(); i != opaqueObjects.end(); i++)
    {

        if((*i) != player) (*i)->render(*levelShader);

    }

    /*
     * Sort transparent objects so blending works correctly
     */
    transparencyObjects.sort(GameObject::pGameObjectComp);

    for(i = transparencyObjects.begin(); i != transparencyObjects.end(); i++)
    {

        if((*i) != player) (*i)->render(*levelShader);

    }

    player->render(*levelShader);

    levelShader->deactivate(ENABLE_POSITION | ENABLE_NORMAL | ENABLE_TEXCOORD);

    // Render health meter
    glColor3f(0.0f, 0.7f, 0.0f);
    float top = ((float)screenheight - 12.0f) / (float)screenheight;
    float left = ((float)screenwidth - 12.0f) / (float)screenwidth;
    glRasterPos3f(-left, -top, 1.0f);
    glCallLists(3, GL_UNSIGNED_BYTE, "100");

    // Render crosshairs
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(0.5f, 0.5f, 0.0f);
    glVertex2f(0.0, 10.0f / screenheight);
    glVertex2f(0.0, 30.0f / screenheight);
    glVertex2f(0.0, -10.0f / screenheight);
    glVertex2f(0.0, -30.0f / screenheight);

    glVertex2f(10.0f / screenwidth, 0.0);
    glVertex2f(30.0f / screenwidth, 0.0);
    glVertex2f(-10.0f / screenwidth, 0.0);
    glVertex2f(-30.0f / screenwidth, 0.0);
    glEnd();
}

void GameState::insertOpaqueObject(GameObject* newGameObject)
{

    levelObjects.push_back(newGameObject);

    GLuint newDiffuseTex = newGameObject->getDiffuseTexture();

    std::list<GameObject*>::iterator it;

    it = opaqueObjects.begin();

    while(it != opaqueObjects.end()){

        if( newDiffuseTex < (*it)->getDiffuseTexture()){

            opaqueObjects.insert(it,newGameObject);
            return;

        }

        it++;
    }

    opaqueObjects.push_back(newGameObject);

}

void GameState::insertTransparencyObject(GameObject* newGameObject)
{

    levelObjects.push_back(newGameObject);

    transparencyObjects.push_back(newGameObject);

}

bool isHovering(WindowContext& ctx, int x, int y)
{
    if (ctx.cursor.x > x && ctx.cursor.x < x + 100 && ctx.cursor.y > y - 18 && ctx.cursor.y < y + 18) {
        return true;
    }
    else {
        return false;
    }
}

void GameState::renderMenu(WindowContext& ctx, MenuAction& action)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    int x = ctx.width / 2 - 50;
    int y = ctx.height / 2 - 50;

    glColor3f(0.5f, 0.5f, 0.5f);
    glRasterPos3f(float(x - ctx.width / 2) / ctx.width, float(ctx.height / 2 - y) / ctx.height, 1.0f);
    glCallLists(15, GL_UNSIGNED_BYTE, "MereEngine Demo");
    y += 36;

    if (isHovering(ctx, x, y)) {
        glColor3f(0.0f, 0.7f, 0.0f);
        if (ctx.cursor.leftButtonDown) {
            action = MenuAction::NEWGAME;
        }
    }
    else {
        glColor3f(0.9f, 0.9f, 0.9f);
    }
    glRasterPos3f(float(x - ctx.width / 2) / ctx.width, float(ctx.height / 2 - y) / ctx.height, 1.0f);
    glCallLists(9, GL_UNSIGNED_BYTE, "New Game");
    y += 36;

    if (isHovering(ctx, x, y)) {
        glColor3f(0.0f, 0.7f, 0.0f);
        if (ctx.cursor.leftButtonDown) {
            action = MenuAction::EXIT;
        }
    }
    else {
        glColor3f(0.9f, 0.9f, 0.9f);
    }
    glRasterPos3f(float(x - ctx.width / 2) / ctx.width, float(ctx.height / 2 - y) / ctx.height, 1.0f);
    glCallLists(5, GL_UNSIGNED_BYTE, "Exit");
    y += 36;

    ctx.cursor.leftButtonDown = false;
}