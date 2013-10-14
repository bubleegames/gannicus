#include "environment.h"
#include "player.h"
#include <utility>

using std::move;

environment::environment()
{
	auto gravity = make_shared<globalForce>();
	gravity->x = 0;
	gravity->y = -6;
	gravity->type = 0;
	gravity->ID = 0;
	gravity->radius = 0;
	gravity->effectCode = 3;
	gravity->posX = 0;
	gravity->posY = 0;
	gravity->length = -1;
	globals.push_back(gravity);
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

void environment::airCheck(instance * a)
{
	if(a->collision.y > floor && a->current.aerial == 0){
		a->current.aerial = 1;
		a->current.reversal = nullptr;
	}
}

void environment::enforce(instance * a)
{
	for(auto& i:globals){
		if(i->validate(a)) i->enforce(*a);
		else std::cout << i->y << ": " << a->pick()->name << '\n';
	}
}
