#include "environment.h"
#include "player.h"
#include <utility>

using std::move;

environment::environment()
{
	auto gravity = make_shared<force>();
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
	for(auto i:globals){
		if(a->validate(i->ID, i->effectCode)) i->enforce(a);
	}
}

void environment::enforceFloor(player * a)
{
	/*Floor, or "Bottom corner"*/

	if (a->collision.y < floor){
		if(a->elasticY){
			a->current.deltaY = -a->current.deltaY;
			a->elasticY = false;
		} else if (a->slide) {
			a->current.deltaY = 0;
			if(a->current.move == a->pick()->untech || a->current.move == a->pick()->die){ 
				if(a->current.deltaX < 0) a->current.deltaX++;
				else if(a->current.deltaX > 0) a->current.deltaX--;
				a->current.aerial = 1;
			} else {
				a->current.deltaX = 0;
				a->slide = 0;
			}
		} else {
			if(a->current.aerial == 1){
				a->land();
				a->updateRects();
				a->current.deltaX = 0;
			}
			a->current.deltaY = 0;
		}
		a->current.posY = floor - a->current.move->collision[a->current.frame].y;
	}
	a->updateRects();
}

