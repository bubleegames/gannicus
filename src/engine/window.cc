#include "window.h"
#include "gl-compat.h"
#include "shader.h"
#include "auxil.h"
#include <SDL/SDL_opengl.h>
#include <iostream>
#include <ctime>

window::window()
{
	screen = nullptr;
	h = 450, w = 800;
	title = "downtop";
}

void window::initShaders()
{
}

void window::screenshot()
{
	time_t now = time(0);
	tm* LT = localtime(&now);
	screenshot("screenshot-"+title+"-"+string(asctime(LT)));
}

void window::screenshot(string filename)
{
	SDL_GL_SwapBuffers();
	glReadPixels(0, 0, w, h, GL_RGBA, ___gufg_tex_mode, image->pixels);
	if(SDL_SaveBMP(image, filename.c_str())) printf("You dun fucked up\n");
}

bool window::screenInit()
{
	if(screen){
		SDL_FreeSurface(screen);
		screen = nullptr;
	}
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Uint32 rmask = 0xff000000;
	Uint32 gmask = 0x00ff0000;
	Uint32 bmask = 0x0000ff00;
	Uint32 amask = 0x000000ff;
#else
	Uint32 rmask = 0x000000ff;
	Uint32 gmask = 0x0000ff00;
	Uint32 bmask = 0x00ff0000;
	Uint32 amask = 0xff000000;
#endif
	image = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
				 rmask, gmask, bmask, amask);

	/*Initialize SDL*/
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		return false;
	}
	/*WM stuff*/
	SDL_WM_SetCaption(title.c_str(), title.c_str());
	if((screen = SDL_SetVideoMode(w, h, 32, SDL_OPENGL | (displayMode ? (displayMode == 2 ? SDL_NOFRAME : SDL_FULLSCREEN) : false))) == nullptr)
		return false;
	SDL_ShowCursor(SDL_DISABLE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable (GL_BLEND);
	glEnable (GL_POINT_SMOOTH);
	glEnable (GL_LINE_SMOOTH);
	//glEnable (GL_POLYGON_SMOOTH);
	glEnable (GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glClearColor(0, 0, 0, 1);
	//glShadeModel (GL_SMOOTH);
	glClearDepth(1.0f);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, h, 0, 1, -1);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glMatrixMode(GL_MODELVIEW);
	std::cout << "GLSL:" << glGetString (GL_SHADING_LANGUAGE_VERSION) << "\n";
	glLoadIdentity();
	initShaders();
	return true;
}

void window::setLighting()
{
	GLfloat lightColor0[] = {0.5f, 0.2f, 0.2f, 0.5f}; 
	GLfloat lightPos0[] = {5.0f, 0.0f, 9.0f, 1.0f}; 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
}
