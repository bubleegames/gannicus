#include "environment.h"
#include "player.h"
#include <utility>

using std::move;

environment::environment()
{
	force gravity;
	gravity.x = 0;
	gravity.y = -6;
	gravity.type = 0;
	gravity.ID = 0;
	gravity.radius = 0;
	gravity.effectCode = 3;
	gravity.posX = 0;
	gravity.posY = 0;
	gravity.length = -1;
	globals.push_back(move(&gravity));
}

void environment::cleanup()
{
	for(unsigned int i = 0; i < globals.size(); i++){
		if(globals[i]->origin){
			if(globals[i]->origin->current.move != globals[i]->check ||
			   globals[i]->origin->current.frame == globals[i]->check->distortSpawn){
				if(globals[i]->length < 0){ 
					globals[i]->origin = nullptr;
					globals[i]->length = -globals[i]->length;
				} else { 
					globals.erase(globals.begin()+i);
					i--;
				}
			}
		} else {
			if (!globals[i]->length) {
				globals.erase(globals.begin()+i);
				i--;
			} else globals[i]->length--;
		}
	}
}



void environment::roundInit()
{
	while(globals.size() > 1)
		globals.pop_back();
}

void environment::enforceGravity(instance * a)
{
	SDL_Rect g; g.x = 0; g.y = grav; g.w = 0; g.h = 0;

	if(a->collision.y > floor && a->current.aerial == 0){
		a->current.aerial = 1;
		a->current.reversal = nullptr;
	}
	else if(a->current.aerial && !a->current.freeze){ 
		a->momentum.push_back(g);
	}
}

void environment::enforceGravity(player * a)
{
	SDL_Rect g; g.x = 0; g.y = grav; g.w = 0; g.h = 0;

	if(a->collision.y > floor && a->current.aerial == 0){
		a->current.aerial = 1;
		a->current.reversal = nullptr;
	}
	else if(a->current.aerial && !a->current.freeze){ 
		if(a->hover > 0 && a->current.deltaY - 6 < 0) g.y = -a->current.deltaY;
		a->momentum.push_back(g);
	}
}

