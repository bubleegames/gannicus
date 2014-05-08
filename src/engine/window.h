#include "shader.h"
#include <SDL/SDL.h>
#include <string>
#ifndef ___window
#define ___window
using std::string;
class window{
public:
	window();
	virtual void initShaders();
	virtual bool screenInit();
	virtual void setLighting();
	virtual void screenshot();
	virtual void screenshot(string);
	shaderProgram prog;
	string title;
	SDL_Surface *screen, *image;
	int displayMode;
	int h, w;
};
#endif
