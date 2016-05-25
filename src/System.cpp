#include "System.h"

System::System(){}

System::~System(){}

System::System(const System & rhs){}

bool System::init(std::string& dir)
{

    rootDir = dir;

    SDL_Init(SDL_INIT_EVERYTHING);

	vidinfo = SDL_GetVideoInfo();

    /*
     * Enable/Disable VSYNC; should be user specified in the future
     */
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,1); //enabled

	/*
	 * Create window with OGL context
	 */
	win = SDL_SetVideoMode(vidinfo->current_w,
	                       vidinfo->current_h,
	                       32,
	                       SDL_OPENGL | SDL_FULLSCREEN);

	#ifdef WIN32
	GLenum err = glewInit(); //required for win32
	#endif // WIN32

    /*
     * Eliminate Cursor & Grab Input
     */
	SDL_ShowCursor(1);
	SDL_WM_GrabInput(SDL_GRAB_ON);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear color to black, no alpha
	glEnable(GL_DEPTH_TEST);              // Enable the depth buffer
	glClearDepth(1.0f);                   // Clear all of the depth buffer
	glDepthFunc(GL_LEQUAL);		          // Depth function; Overwrite if new value <= current value
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    try
    {
        mainmenu.init(rootDir,200.0f);
    }
    catch (std::exception & e)
    {
        std::cerr << e.what() << "\n";
        return false;
    }

    try
    {
        state.init(rootDir,200.0f);
    }
    catch (std::exception & e)
    {
        std::cerr << e.what() << "\n";
        return false;
    }

    return true;

}

bool System::loop()
{

    //firstlevel string will be determined by game files
    std::string firstLevel = std::string("Data/Test.level");

    /*
     * Switch between Menu loop and Game loop
     */
    while(mainmenu.loop()) //If return 0, exit program
    {

        if(mainmenu.currentAction == MENU_ACTION_NEWGAME)
        {

            /*
             * create a new game from beginning
             */

            if(state.loaded) state.clean();

            try{

                state.loadNew(rootDir+firstLevel);

            }
            catch(std::exception& e)
            {

                std::cerr << e.what() << "\n";
                return false;

            }

        }
        if(mainmenu.currentAction == MENU_ACTION_NULL)
        {
            break;
        }

        state.loop();

    }

    return true;

}
