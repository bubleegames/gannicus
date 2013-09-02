#include "window.h"
#include "gl-compat.h"
#include "shader.h"
#include "auxil.h"
#include <SDL/SDL_opengl.h>
#include <iostream>

window::window()
{
	screen = nullptr;
	h = 450, w = 800;
}

void window::initShaders()
{
}

bool window::screenInit()
{
	/*Initialize SDL*/
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		return false;
	}
	/*WM stuff*/
	SDL_WM_SetCaption("downtop", "downtop");
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
