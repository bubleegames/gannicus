#include "environment.h"
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
	for(unsigned int i = 0; i < env.globals.size(); i++){
		if(env.globals[i]->origin){
			if(env.globals[i]->origin->current.move != env.globals[i]->check ||
			   env.globals[i]->origin->current.frame == env.globals[i]->check->distortSpawn){
				if(env.globals[i]->length < 0){ 
					env.globals[i]->origin = nullptr;
					env.globals[i]->length = -env.globals[i]->length;
				} else { 
					env.globals.erase(env.globals.begin()+i);
					i--;
				}
			}
		} else {
			if (!env.globals[i]->length) {
				env.globals.erase(env.globals.begin()+i);
				i--;
			} else env.globals[i]->length--;
		}
	}
}
