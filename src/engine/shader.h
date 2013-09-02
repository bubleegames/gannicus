#ifndef SHADERS_INCLUDED
#define SHADERS_INCLUDED
#include "gl-compat.h"
#include <SDL/SDL_opengl.h>
#include <vector>
#include <string>
using std::string;
using std::vector;
class shader {
public:
	GLuint x;
	virtual void init(string);
};

class vertShader : public shader {
public:
	vertShader(string);
};

class fragShader : public shader {
public:
	fragShader(string);
};

class geoShader : public shader {
public:
	geoShader(string);
};

class shaderProgram {
public:
	virtual void init();
	GLuint handle;
	shaderProgram(string, string);
	shaderProgram() {}
	GLuint operator()();
};
#endif
