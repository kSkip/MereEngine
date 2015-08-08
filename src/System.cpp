#include "System.h"

System::System(){}

System::~System(){}

System::System(const System & rhs){}

bool System::init(){

    SDL_Init(SDL_INIT_EVERYTHING);

	vidinfo = SDL_GetVideoInfo();

	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,0); //Disable VSYNC
	win = SDL_SetVideoMode(vidinfo->current_w,vidinfo->current_h,32,SDL_OPENGL | SDL_FULLSCREEN); //create window and ogl context

	#ifdef WIN32
	GLenum err = glewInit(); //required for win32
	#endif // WIN32

    //some SDL initializations for eliminating cursor, and grabbing input
	SDL_ShowCursor(1);
	SDL_WM_GrabInput(SDL_GRAB_ON);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);              // Set out clear color to black, no alpha
	glEnable(GL_DEPTH_TEST);                           // Enable the depth buffer
	glClearDepth(1.0f);                                // Clear the entire depth of the depth buffer
	glDepthFunc(GL_LEQUAL);		                       // Set our depth function to overwrite if new value less than or equal to current value
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	state.init(300.0f); //initialize the GameState at 300fps max
    mainmenu.init(200.0f); //initialize the menuObject at 200fps max

    return true;
}

bool System::loop(){

    //here we switch between the menu loop and the game loop
    while(mainmenu.loop()){ //if returns 1 then escape the gamestate's loop, if returns 0 then the we are quiting the program and sys loop returns

        if(mainmenu.currentAction == MENU_ACTION_NEWGAME){
            if(state.loaded){
                state.clean();
            }
            state.loadNew(std::string("Data/Test.level")); //load the new level data for the state
        }

        state.loop();

    }

    return true;
}
