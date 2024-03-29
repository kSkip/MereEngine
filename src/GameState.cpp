#include <numeric>

#include "GameState.h"
#include "GameObjects/StaticObject.h"
#include "GameObjects/Character.h"
#include "GameObjects/SkyBox.h"
#include "GameObjects/Bullet.h"
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

    for(i = levelObjects.begin(); i != levelObjects.end(); i++)
        delete (*i);

    levelObjects.clear();

    opaqueObjects.clear();
    transparencyObjects.clear();

}

GameState::GameState(const GameState & rhs){}

void GameState::init(std::string dir)
{
    manager.build(dir);

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

    for(i = levelObjects.begin(); i != levelObjects.end(); i++)
        delete (*i);

    levelObjects.clear();

    opaqueObjects.clear();
    transparencyObjects.clear();

    manager.clear();

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

void GameState::loadNew(std::string levelFilePath)
{    
    std::string vs = manager.getVertexShader("demo");
    std::string fs = manager.getFragmentShader("demo");

    shader.loadShader(vs.c_str(),fs.c_str());

    player.bounds.LoadBoundaries(manager.getBoundsFilePath("Camera").c_str());

    insertOpaqueObject(new StaticObject(manager.getStaticModel("Ground")));
    insertOpaqueObject(new StaticObject(manager.getStaticModel("Walls")));
    insertOpaqueObject(new StaticObject(manager.getStaticModel("ShedFloor")));
    insertOpaqueObject(new StaticObject(manager.getStaticModel("ShedWalls")));
    insertOpaqueObject(new StaticObject(manager.getStaticModel("ShedRoof")));

    insertOpaqueObject(new SkyBox(player, manager.getStaticModel("SkyBox")));

    insertOpaqueObject(new Character(vec3(3.0f, 2.0f, 0.0f), player, manager.getAnimatedModel("Character")));

    player.weapon.model = manager.getAnimatedModel("Gun");

    loaded = true;
}

void GameState::loadSave(std::string savefile){}

void GameState::save(){}

void GameState::movingForward(bool isMoving)
{
    player.holdingForward = isMoving;
}

void GameState::movingBackward(bool isMoving)
{
    player.holdingBackward = isMoving;
}

void GameState::movingLeft(bool isMoving)
{
    player.holdingLeftStrafe = isMoving;
}

void GameState::movingRight(bool isMoving)
{
    player.holdingRightStrafe = isMoving;
}

void GameState::handleSpacebar()
{
    if (player.onGround()) {
        player.setYSpeed(1.5f);
        player.isNotGrounded();
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
    Weapon& weapon = player.weapon;
    if (!weapon.isFiring) {

        vec3 Begin = player.camera.getHead() + 0.1f * player.camera.getFront();
        vec3 End = player.camera.getHead() + 100.0f * player.camera.getFront();
        GameObject* newobject = new Bullet(Begin, End, &player);

        levelObjects.push_back(newobject);
        opaqueObjects.push_back(newobject);

        weapon.fire();
    }
}

bool GameState::run(double elapsedTime)
{
    elapsedTime = elapsedTime > 0.1 ? 0.1 : elapsedTime;

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
    player.camera.handleMouseMove(context.cursor.dx, context.cursor.dy);
    context.cursor.dx = 0;
    context.cursor.dy = 0;

    std::list<GameObject*>::iterator i, j;

    player.move(deltatime);

    for(i = levelObjects.begin(); i != levelObjects.end(); i++)
        (*i)->move(deltatime);


    for(i = levelObjects.begin(); i != levelObjects.end(); i++)
    {
        GameObject::testResolveCollision((*i), &player);
        for(j = i; j != levelObjects.end(); j++)
        {
            if(i != j) GameObject::testResolveCollision((*i),(*j)); //detect and resolve collisions
        }
    }

    player.commitMovement();
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

void GameState::render()
{

    std::map<std::string,GLuint>::iterator textureIt;
    std::list<GameObject*>::iterator i;
    mat4 perspectiveMatrix;
    mat4 viewMatrix;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    perspectiveMatrix = perspective(1.04f, aspectRatio, 0.1f, 1000.0f);
    player.camera.getViewMatrix(viewMatrix);
    
    glUseProgram(shader.program);

    glUniformMatrix4fv(shader.modelView, 1, GL_FALSE, (GLfloat*)&viewMatrix);
    glUniformMatrix4fv(shader.projection, 1, GL_FALSE, (GLfloat*)&perspectiveMatrix);


    //Use of the only the first texture only for now
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(shader.texture, 0);

    /*
     * Render the opaque objects first t ofill the depth buffer
     * Then the transparent obejcts will go next
     */
    for(i = opaqueObjects.begin(); i != opaqueObjects.end(); i++) {
        (*i)->draw(shader);
    }

    /*
     * Sort transparent objects so blending works correctly
     */
    transparencyObjects.sort(GameObject::pGameObjectComp);

    for(i = transparencyObjects.begin(); i != transparencyObjects.end(); i++) {
        (*i)->draw(shader);
    }

    glClear(GL_DEPTH_BUFFER_BIT);

    viewMatrix[0] = vec4(1.0f, 0.0f, 0.0f, 0.0f);
    viewMatrix[1] = vec4(0.0f, 1.0f, 0.0f, 0.0f);
    viewMatrix[2] = vec4(0.0f, 0.0f, 1.0f, 0.0f);
    viewMatrix[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(shader.modelView, 1, GL_FALSE, (GLfloat*)&viewMatrix);

    player.weapon.draw(shader);

    glUseProgram(0);

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