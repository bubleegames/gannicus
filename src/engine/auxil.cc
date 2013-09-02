/*Copyright Somnambulant Studios 2012*/
#include "auxil.h"
#include "gl-compat.h"
#include "sdl-compat.h"
#include "tokenizer.h"
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <vector>
using std::max;
using std::min;
SDL_Surface* aux::init_screen(int width, int height, int bpp) {
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		return nullptr;
	}

	SDL_Surface* screen = SDL_SetVideoMode(width, height, bpp, SDL_SWSURFACE);

	SDL_WM_SetCaption("Someday soon I'll be a game!", nullptr);

	return screen;
}

void aux::update_screen(SDL_Surface* source, SDL_Surface* destination) {
	SDL_Surface* scaled2x = scale2x(source);
	scale(scaled2x, destination);
	SDL_FreeSurface(scaled2x);

	//scale(source, destination);
	SDL_Flip(destination);
}

string aux::textFileRead(string filename)
{
	std::ifstream read;
	read.open(filename);
	string buf, ret;
	while(std::getline(read, buf))
		ret += buf + "\n";
	read.close();
	return ret;
}

// uses nearest-neighbor algorithm found here:
// http://tech-algorithm.com/articles/nearest-neighbor-image-scaling/
void aux::scale(SDL_Surface* source, SDL_Surface* destination) {
	SDL_LockSurface(source);
	SDL_LockSurface(destination);

	Uint32* source_pixels      = (Uint32*)source->pixels;
	Uint32* destination_pixels = (Uint32*)destination->pixels;

	int w1 = source->w;
	int h1 = source->h;
	int w2 = destination->w;
	int h2 = destination->h;

	double x_ratio = w1 / (double)w2;
	double y_ratio = h1 / (double)h2;

	for (int i = 0; i < h2; i++) {
		for (int j = 0; j < w2; j++) {
			int px = floor(j*x_ratio);
			int py = floor(i*y_ratio);

			destination_pixels[i*w2 + j] = source_pixels[(int)(py*w1 + px)];
		}
	}

	SDL_UnlockSurface(source);
	SDL_UnlockSurface(destination);
}

void aux::bilinear_scale(SDL_Surface* source, SDL_Surface* destination) {
	SDL_LockSurface(source);
	SDL_LockSurface(destination);

	Uint32* source_pixels      = (Uint32*)source->pixels;
	Uint32* destination_pixels = (Uint32*)destination->pixels;

	int w1 = source->w;
	int h1 = source->h;
	int w2 = destination->w;
	int h2 = destination->h;

	double x_ratio = w1 / (double)w2;
	double y_ratio = h1 / (double)h2;

	SDL_PixelFormat* format = source->format;

	for (int i = 0; i < h2; i++) {
		for (int j = 0; j < w2; j++) {
			int px = floor(j*x_ratio);
			int py = floor(i*y_ratio);

			int num_nbrs = 0;
			int red = 0;
			int green = 0;
			int blue = 0;

			if (py > 0) {
				num_nbrs++;

				Uint32 pixel = source_pixels[(int)((py-1)*w1 + px)];

				red   += (pixel & format->Rmask) >> format->Rshift;
				green += (pixel & format->Gmask) >> format->Gshift; 
				blue  += (pixel & format->Bmask) >> format->Bshift; 
			}

			if (py < h1-1) {
				num_nbrs++;

				Uint32 pixel =  source_pixels[(int)((py+1)*w1 + px)];

				red   += (pixel & format->Rmask) >> format->Rshift;
				green += (pixel & format->Gmask) >> format->Gshift; 
				blue  += (pixel & format->Bmask) >> format->Bshift; 
			}

			if (px > 0) {
				num_nbrs++;

				Uint32 pixel =  source_pixels[(int)(py*w1 + (px-1))];

				red   += (pixel & format->Rmask) >> format->Rshift;
				green += (pixel & format->Gmask) >> format->Gshift; 
				blue  += (pixel & format->Bmask) >> format->Bshift; 
			}

			if (px < w1-1) {
				num_nbrs++;

				Uint32 pixel = source_pixels[(int)(py*w1 + (px+1))];

				red   += (pixel & format->Rmask) >> format->Rshift;
				green += (pixel & format->Gmask) >> format->Gshift; 
				blue  += (pixel & format->Bmask) >> format->Bshift; 
			}

			destination_pixels[i*w2 + j] = SDL_MapRGB(format, red / num_nbrs, green / num_nbrs, blue / num_nbrs); 
		}
	}

	SDL_UnlockSurface(source);
	SDL_UnlockSurface(destination);
}

// uses scale 2x algorithm to create a smoother-looking result
// https://secure.wikimedia.org/wikipedia/en/wiki/Pixel_art_scaling_algorithms
SDL_Surface* aux::scale2x(SDL_Surface* source) {
	int w1 = source->w;
	int h1 = source->h;

	SDL_Surface* ret = SDL_CreateRGBSurface(SDL_SWSURFACE, w1*2, h1*2, 32, 0, 0, 0, 0);
	int w2 = ret->w;
	//int h2 = ret->h; //FIXME Unused Variable

	SDL_LockSurface(source);
	SDL_LockSurface(ret);

	Uint32* source_pixels = (Uint32*)source->pixels;
	Uint32* ret_pixels    = (Uint32*)ret->pixels;

	for (int i = 0; i < h1; i++) {
		for (int j = 0; j < w1; j++) {
			Uint32 a = i > 0      ? (Uint32)source_pixels[(i-1) * w1 + j] : 0; 
			Uint32 b = j < w1 - 1 ? (Uint32)source_pixels[i * w1 + (j+1)] : 0; 
			Uint32 c = j > 0      ? (Uint32)source_pixels[i * w1 + (j-1)] : 0; 
			Uint32 d = i < h1 - 1 ? (Uint32)source_pixels[(i+1) * w1 + j] : 0; 

			ret_pixels[(i*2) * w2 + (j*2)]         = (Uint32)source_pixels[i * w1 +j];
			ret_pixels[(i*2) * w2 + (j*2 + 1)]     = (Uint32)source_pixels[i * w1 +j];
			ret_pixels[(i*2 + 1) * w2 + (j*2 + 1)] = (Uint32)source_pixels[i * w1 +j];
			ret_pixels[(i*2 + 1) * w2 + (j*2)]     = (Uint32)source_pixels[i * w1 +j];

			if (c == a && c != d && a != b) {
				ret_pixels[(i*2) * w2 + (j*2)] = a;
			}

			if (a == b && a != c && b != d) {
				ret_pixels[(i*2) * w2 + (j*2 + 1)] = b;
			}

			if (b == d && b != a && d != c) {
				ret_pixels[(i*2 + 1) * w2 + (j*2 + 1)] = d;
			}

			if (d == c && d != b && c != a) {
				ret_pixels[(i*2 + 1) * w2 + (j*2)] = c;
			}
		}
	}

	SDL_UnlockSurface(source);
	SDL_UnlockSurface(ret);

	return ret;
}

// loads an image from a file name and returns it as a surface
SDL_Surface* aux::load_image(string filename) {
	SDL_Surface* loadedImage    = nullptr;
	SDL_Surface* optimizedImage = nullptr;

	loadedImage = IMG_Load(filename.c_str());

	// if nothing went wrong on load
	if (loadedImage != nullptr) {
		//Create an optimized image
		optimizedImage = SDL_DisplayFormatAlpha(loadedImage);

		//Free the old image
		SDL_FreeSurface(loadedImage);
	}

	return optimizedImage;
}

// applies one surface to another based on x and y coords
void aux::apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination) {
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;

	SDL_BlitSurface(source, nullptr, destination, &offset);
}

SDL_Rect aux::collisionRect(SDL_Rect a, SDL_Rect b)
{
	SDL_Rect ret = {0, 0, 0, 0};
	int leftX = max(a.x, b.x);
	int rightX = min(a.x+a.w, b.x+b.w);
	int bottomY = max(a.y, b.y);
	int topY = min(a.y+a.h, b.y+b.h);
	ret.x = leftX; ret.y = bottomY; 
	ret.w = rightX - leftX > 0 ? rightX - leftX : 0;
	ret.h = topY - bottomY > 0 ? topY - bottomY : 0;
	return ret;
}

bool aux::checkCollision(SDL_Rect a, SDL_Rect b, SDL_Rect &c)
{
	c = collisionRect(a, b);
	return (c.w && c.h);
}

bool aux::checkCollision(SDL_Rect a, SDL_Rect b)
{
	if(a.h == 0 || a.w == 0 || b.h == 0 || b.w == 0) return 0;
	if(a.y + a.h - b.y <= 0 || b.y + b.h - a.y <= 0) return 0;
	if(a.x + a.w - b.x <= 0 || b.x + b.w - a.x <= 0) return 0;
	return 1;
}

vector<SDL_Rect> aux::defineRectArray(string definition)
{
	vector<SDL_Rect> ret;
	vector<string> coordinate;
	tokenizer t(definition, " \n\t");
	coordinate.push_back(t());
	while(t().size())
		coordinate.push_back(t.current());

	for(unsigned int i = 0; i < coordinate.size(); i+=4){
		SDL_Rect t;
		t.x = stoi(coordinate[i]);
		t.y = stoi(coordinate[i + 1]);
		t.w = stoi(coordinate[i + 2]);
		t.h = stoi(coordinate[i + 3]);
		ret.push_back(t);
	}
	return ret;
}

vector<pixelMap> aux::surface_to_normals(SDL_Surface * source)
{
	unsigned char * p = (unsigned char*)source->pixels;
	int x = 0;
	vector<pixelMap> ret;
	pixelMap temp;
	for(int i = 0; i < source->h; i++){
		for(int j = 0; j < source->w; j++){
			temp.normal.x = (p[x] - 128) / 128.0;
			temp.normal.y = (p[x+1] - 128) / 128.0;
			temp.normal.z = (p[x+2] - 128) / 128.0;
			temp.colorIndex = p[x+3];
			x += 4;
			ret.push_back(temp);
		}
	}
	return ret;
}

GLuint aux::surface_to_texture(SDL_Surface * source, unsigned int * pixels)
{
	if(source == nullptr) return -1;
	GLint nColors;
	GLenum texFormat;
	GLuint texture;
	nColors = source->format->BytesPerPixel;
	if (source->format->Rmask == 0x000000ff)
		texFormat = GL_RGBA;
	else
		texFormat = GL_BGRA;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, nColors, source->w, source->h, 0, texFormat, ___gufg_tex_mode, pixels);
	return texture;
}

GLuint aux::surface_to_texture(SDL_Surface * source)
{
	if(source == nullptr) return -1;
	GLint nColors;
	GLenum texFormat;
	GLuint texture;
	nColors = source->format->BytesPerPixel;
	if (source->format->Rmask == 0x000000ff)
		texFormat = GL_RGBA;
	else
		texFormat = GL_BGRA;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, nColors, source->w, source->h, 0, texFormat, ___gufg_tex_mode, source->pixels);
	return texture;
}

GLuint aux::load_texture (string filename)
{
	SDL_Surface * i = load_image(filename);
	if(!i) return 0;
	else return surface_to_texture(i);
}

void vect::unitNormal(float Ax, float Ay, float Az, float Bx, float By, float Bz, float Cx, float Cy, float Cz)
{
	vect AB, AC;
	float i, j, k, len;
	AB.x = Bx - Ax;
	AB.y = By - Ay;
	AB.z = Bz - Az;
	AC.x = Cx - Ax;
	AC.y = Cy - Ay;
	AC.z = Cz - Az;
	i = (AB.y*AC.z-AC.y*AB.z); j = -(AB.x*AC.z-AC.x*AB.z); k = (AB.x*AC.y-AC.x*AB.y);
	len = sqrt(i*i + j*j + k*k);
	x = i / len;
	y = j / len;
	z = k / len;
}

status::status()
{
	deltaX = 0;
	deltaY = 0;
	lCorner = 0;
	rCorner = 0;
	frame = 0;
	connect = 0;
	counter = 0;
	hit = 0;
	reversalTimer = 0;
	move = nullptr;
	bufferedMove = nullptr;
	reversal = nullptr;
	freeze = 0;
	aerial = false;
	dead = false;
}

status::status(const status & o) 
{
	posX = o.posX; posY = o.posY;
	drawX = o.drawX; drawY = o.drawY;
	facing = o.facing;
	deltaX = o.deltaX; deltaY = o.deltaY; 
	frame = o.frame;
	freeze = o.freeze;
	reversalTimer = o.reversalTimer;
	aerial = o.aerial;
	rCorner = o.rCorner; lCorner = o.lCorner;
	move = o.move; bufferedMove = o.bufferedMove; reversal = o.reversal;
	connect = o.connect; hit = o.hit; counter = o.counter;
	dead = o.dead;
	throwInvuln = o.throwInvuln;
	reversalFlag = o.reversalFlag;
	meter = o.meter;
	offspring = o.offspring;
	age = o.age;
	dead = o.dead;
	mode = o.mode;
	prox = o.prox;
}

