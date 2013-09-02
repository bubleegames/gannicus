#include "shader.h"
#include <SDL/SDL.h>
#ifndef ___window
#define ___window

class window{
public:
	window();
	virtual void initShaders();
	virtual bool screenInit();
	virtual void setLighting();
	shaderProgram prog;
	SDL_Surface * screen;
	int displayMode;
	int h, w;
};
#endif
