#include "gameinstance.h"
#include <assert.h>
void gameInstance::init()
{
	assert(screenInit());
	harness::init();
	camY = 0.0; tuY = 0; tdY = 0;
	camX = 0.0; tuX = 0; tdX = 0;
	camZ = -5.0; tuZ = 0; tdZ = 0;
	size = 1.0;
	pan = 0.0; rlX = 0; rrX = 0;
	tilt = 0.0; rlY = 0; rrY = 0;
}

void gameInstance::processInput(SDL_Event &event)
{
	/*Do stuff with event*/
	switch (event.type){
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		switch (event.key.keysym.sym) {
		case SDLK_w:
			printf("%d\n", camZ);
			tuZ = !tuZ;
			break;
		case SDLK_s:
			printf("%d\n", camZ);
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
		case SDLK_e:
			rlY = !rlY;
			break;
		case SDLK_r:
			rrY = !rrY;
			break;
		case SDLK_z:
			size -= 1.0f;
			break;
		case SDLK_x:
			size += 1.0f;
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

void gameInstance::readInput()
{
	harness::readInput();
	if(tuX) camX += 0.1f;
	if(tuY) camY += 0.1f;
	if(tuZ) camZ += 0.1f;
	if(tdX) camX -= 0.1f;
	if(tdY) camY -= 0.1f;
	if(tdZ) camZ -= 0.1f;
	if(rlX) pan -= 0.1f;
	if(rlY) tilt -= 0.1f;
	if(rrX) pan += 0.1f;
	if(rrY) tilt += 0.1f;
}
