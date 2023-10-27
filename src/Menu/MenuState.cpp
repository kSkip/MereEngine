#include "MenuState.h"

#include "Utilities/DataBlock.h"
#include "Models/BasicGLBufferUtilities.h"

MenuState::MenuState(){}

MenuState::~MenuState(){}

MenuState::MenuState(const MenuState & rhs){}

void MenuState::init(std::string dir, int width, int height)
{

    int screenwidth = width;
    int screenheight = height;

    //TTF_Init();

    rootDir = dir;

    std::string menulistFile;

    /*
     * Should be changed in the future
     * to be any filename
     */
    menulistFile = rootDir + "Data/Main.menu";
    std::cout << menulistFile << "\n";

    /*
     * Now load the menu data
     * from menulistFile
     */
    DataBlock menudef;

    menudef.load(menulistFile.c_str());

    /*
     * default font must exist
     */
    defaultFont = menudef("defaultfont");

    if(defaultFont.empty())
        throw std::runtime_error("Menu definition requires a default font");

    /*
     * vertex and fragment shaders must
     * be defined
     */
    std::string vertexFile, fragmentFile;

    vertexFile   = rootDir + menudef("vs");

    fragmentFile = rootDir + menudef("fs");

    try
    {

        menuShader = new Shader;

        menuShader->loadShader(vertexFile,fragmentFile);

    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << "\n";
        delete menuShader;
        throw std::runtime_error("Failed to init menu shader");
    }

    /*
     * now go through the menu
     * defining blocks add then
     * one by one
     */
    block_it it = menudef.beginBlocks();

    while(it != menudef.endBlocks())
    {

        DataBlock & item = it->second;

        if(item("font").empty())
            item("font") = rootDir + defaultFont;
        else
            item("font") = rootDir + item("font");

        MenuList.push_back( new MenuItem(item) );

        ++it;

    }

}

void MenuState::renderMenu()
{

    unsigned int i;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    menuShader->activate(ENABLE_POSITION | ENABLE_NORMAL | ENABLE_TEXCOORD);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(menuShader->texture, 0);


    for(i=0;i<MenuList.size();i++){

    glBindTexture(GL_TEXTURE_2D,MenuList[i]->tex);

    glUniformMatrix4fv(menuShader->translation,1, GL_FALSE,glm::value_ptr(MenuList[i]->translation));
    glUniformMatrix4fv(menuShader->scale,1, GL_FALSE,glm::value_ptr(MenuList[i]->scale));

    glUniform1ui(menuShader->mouseOver,MenuList[i]->mouseOver);

    glBindBuffer(GL_ARRAY_BUFFER, MenuList[i]->vertexBuffer);

    glVertexAttribPointer(
        LOCATION_POSITION,
        3, GL_FLOAT, GL_FALSE,sizeof(struct vertex),
        (void*)offsetof(struct vertex, position)
    );

    glVertexAttribPointer(
        LOCATION_TEXCOORD,
        2, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
        (void*)offsetof(struct vertex, texcoord)
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MenuList[i]->elementBuffer);

    glDrawElements(
        GL_TRIANGLES,
        6,
        GL_UNSIGNED_INT,
        (void*)0
    );

    }

    menuShader->deactivate(ENABLE_POSITION | ENABLE_NORMAL | ENABLE_TEXCOORD);

}

void MenuState::handleButtonDown()
{
    for (int i = 0; i < MenuList.size(); i++) {
        if (MenuList[i]->mouseOver) {
            switch (MenuList[i]->action) {
            case MENU_ACTION_NULL:
                break;
            case MENU_ACTION_NEWGAME:
                currentAction = MENU_ACTION_NEWGAME;
                break;
            case MENU_ACTION_QUIT:
                currentAction = MENU_ACTION_QUIT;
                break;
            default:
                break;
            }
        }
    }
}

void MenuState::handleMouseMove(int x, int y)
{
    for (int i = 0; i < MenuList.size(); i++) {
        MenuList[i]->handleMouseMove(x, y, screenwidth, screenheight);
    }
}

MenuState::MenuItem::MenuItem(){}

MenuState::MenuItem::MenuItem(DataBlock & def)
{
    /*
     * dimensions are comma separated
     */
    strvec valueStr = split(def("dimensions"),',');

    float x = atof(valueStr.at(0).c_str());
    float y = atof(valueStr.at(1).c_str());
    float yscale = atof(valueStr.at(2).c_str());

    /* strings need their quotes removed */
    std::string itemLabel = trim(def("label"),'\"');
    const char* label = itemLabel.c_str();

    translation = glm::translate(glm::mat4(1.0f),glm::vec3(x,y,0.0f));

    vertexBuffer = basicQuadVertexBuffer();

    elementBuffer = basicQuadElementBuffer();

    std::string trueTypeFile = def("font");

    /*
     * create the texture containing
     * the specified string
     */
    float aspect = 0.0f;
    //TODO: Re-implement the text
    /*
    TTF_Font* font = TTF_OpenFont(trueTypeFile.c_str(), 20);
    

    SDL_Color text_color = {127, 0, 0};

    tex = GenTextToTexture(label,font,text_color,&aspect);

    TTF_CloseFont(font);
    */

    scale = glm::scale(glm::mat4(1.0f),glm::vec3( aspect * yscale, yscale, 0.0f));

    mouseOver = false;

    /*
     * menu objects with actions are
     * indicated by the clickable flag
     */
    if(def("clickable") == "yes")
    {

        if(def("action") == "loadnew") action = MENU_ACTION_NEWGAME;

        else if(def("action") == "quit") action = MENU_ACTION_QUIT;

    }
    else
        action = MENU_ACTION_NULL;

}

void MenuState::MenuItem::handleMouseMove(int mouseX, int mouseY, int width, int height){

    if(this->action == 0) return;

    float xscaled = -2.0f*(float(mouseX) / float(width))+1.0f;
    float yscaled = -2.0f*(float(mouseY) / float(height))+1.0f;

    float yHigh = (this->translation * this->scale * glm::vec4(0.0f,1.0f,0.0f,1.0f)).y;
    float yLow  = (this->translation * this->scale * glm::vec4(0.0f,-1.0f,0.0f,1.0f)).y;

    float xHigh = (this->translation * this->scale * glm::vec4(1.0f,0.0f,0.0f,1.0f)).x;
    float xLow  = (this->translation * this->scale * glm::vec4(-1.0f,0.0f,0.0f,1.0f)).x;

    if(xscaled > xLow && xscaled < xHigh && yscaled > yLow && yscaled < yHigh){
        this->mouseOver = true;
    }else{
        this->mouseOver = false;
    }

}

