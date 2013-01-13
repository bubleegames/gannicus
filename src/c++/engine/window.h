#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glut.h>
#ifndef WINDOW_INCLUDED
#define WINDOW_INCLUDED
class window{
public:
	window();
	virtual bool screenInit();
	virtual void setLighting();
	SDL_Surface * screen;
	int h, w;
};
#endif
