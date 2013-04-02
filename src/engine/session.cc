#include "session.h"

void session::init()
{
	screenInit();
	harness::init();
	camInit();
}

void session::camInit()
{
	camY = 0.0; tuY = 0; tdY = 0;
	camX = 0.0; tuX = 0; tdX = 0;
	camZ = -5.0; tuZ = 0; tdZ = 0;
	pan = 0.0; rlX = 0; rrX = 0;
	tilt = 0.0; rlY = 0; rrY = 0;
	spin = 0.0; rlZ = 0; rrZ = 0;
	r = 0.5; g = 0.5; b = 0.5;
}

void session::processInput(SDL_Event &event)
{
	switch (event.type){
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		switch (event.key.keysym.sym) {
		case SDLK_w:
			tuZ = !tuZ;
			break;
		case SDLK_s:
			tdZ = !tdZ;
			break;
		case SDLK_d:
			tuX = !tuX;
			break;
		case SDLK_a:
			tdX = !tdX;
			break;
		case SDLK_UP:
			tuY = !tuY;
			break;
		case SDLK_DOWN:
			tdY = !tdY;
			break;
		case SDLK_LEFT:
			rlX = !rlX;
			break;
		case SDLK_RIGHT:
			rrX = !rrX;
			break;
		case SDLK_h:
			rlY = !rlY;
			break;
		case SDLK_l:
			rrY = !rrY;
			break;
		case SDLK_j:
			rlZ = !rlZ;
			break;
		case SDLK_k:
			rrZ = !rrZ;
			break;
		case SDLK_SPACE:
			camInit();
			break;
		case SDLK_r:
			if(r < 1) r += 0.05f;
			else{
				if(g > 0.0f) g -= 0.05;
				if(b > 0.0f) b -= 0.05;
			}
			break;
		case SDLK_g:
			if(g < 1) g += 0.05f;
			else{
				if(r > 0.0f) r -= 0.05;
				if(b > 0.0f) b -= 0.05;
			}
			break;
		case SDLK_b:
			if(b < 1) b += 0.05f;
			else{
				if(r > 0.0f) r -= 0.05;
				if(g > 0.0f) g -= 0.05;
			}
			break;
		default:
			harness::processInput(event);
			break;
		}
	default:
		harness::processInput(event);
		break;
	}
}

void session::readInput()
{
	harness::readInput();
	if(tuX) camX += 0.1f;
	if(tuY) camY += 0.1f;
	if(tuZ) camZ += 0.1f;
	if(tdX) camX -= 0.1f;
	if(tdY) camY -= 0.1f;
	if(tdZ) camZ -= 0.1f;
	if(rlX) pan -= 1.0f;
	if(rlY) tilt -= 1.0f;
	if(rlZ) spin -= 1.0f;
	if(rrX) pan += 1.0f;
	if(rrY) tilt += 1.0f;
	if(rrZ) spin += 1.0f;
}
