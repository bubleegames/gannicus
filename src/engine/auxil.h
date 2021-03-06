/*Copyright Ashley Fisher 2012
 *Modified with permission by Somnambulant Studios, 2012-2013*/
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <string>
#include <vector>
#include "span.h"
#include "status.h"
#include "hud.h"

#ifndef ___auxil
#define ___auxil

using std::string;
using std::vector;

class vect{
public:
	float x, y, z;
	void unitNormal(float, float, float, float, float, float, float, float, float);
};

struct pixelMap{
	vect normal;
	int colorIndex;
};

class aux{
	private:
		static void scale  (SDL_Surface* source, SDL_Surface* destination);
		static void bilinear_scale  (SDL_Surface* source, SDL_Surface* destination);
		static SDL_Surface* scale2x(SDL_Surface* source);
	public:
		static string textFileRead(string filename);
		static GLuint load_texture(string filename); 
		static GLuint surface_to_texture(SDL_Surface* source);
		static GLuint surface_to_texture(SDL_Surface* source, unsigned int *);
		static vector<pixelMap> surface_to_normals(SDL_Surface *);
		static SDL_Surface* init_screen(int width, int height, int bpp);
		static void update_screen(SDL_Surface* source, SDL_Surface* destination);

		static SDL_Surface* load_image(string filename);
		static void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination);
		static bool checkCollision(SDL_Rect, SDL_Rect);
		static bool checkCollision(SDL_Rect, SDL_Rect, SDL_Rect&);
		static SDL_Rect collisionRect(SDL_Rect, SDL_Rect);
		static vector<SDL_Rect> defineRectArray(string);
};
#endif
