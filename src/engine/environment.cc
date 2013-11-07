#include "environment.h"
#include "player.h"
#include <utility>

using std::move;
using std::ifstream;

environment::environment()
{
	load("world");
	auto gravity = make_shared<globalForce>();
	gravity->x = 0;
	gravity->y = -6;
	gravity->type = 0;
	gravity->ID = 0;
	gravity->radius = 0;
	gravity->effectCode = 3;
	gravity->current.posX = 0;
	gravity->current.posY = 0;
	gravity->length = -1;
	physics.push_back(gravity);
}

void environment::load(string name)
{
	ifstream read;
	read.open("content/stages/"+name+"/"+name+".st");
	char buffer[1024];
	do {
		read.getline(buffer, 1000);
		setParameter(buffer);
	} while(!read.eof());
	buffer[0] = '\0';
}

void environment::setParameter(string param)
{
	tokenizer t(param, "\t:\n");
	if(t() == "Name"){
	} else if (t.current() == "Camera") {
		screenWidth = stoi(t(" x:\n\t"));
		screenHeight = stoi(t());
		return;
	} else if(t.current() == "Bounds") {
		wall = stoi(t(" x:\n\t"));
		floor = stoi(t());
		return;
	} else if (t.current() == "Size") {
		bg.w = stoi(t(" x:\n\t"));
		bg.h = stoi(t());
		return;
	}
}

void environment::cleanup()
{
	for(unsigned int i = 0; i < physics.size(); i++){
		if(physics[i]->origin){
			if(physics[i]->origin->current.move != physics[i]->check ||
			   physics[i]->origin->current.frame == physics[i]->check->distortSpawn){
				if(physics[i]->length < 0){ 
					physics[i]->origin = nullptr;
					physics[i]->length = -physics[i]->length;
				} else { 
					physics.erase(physics.begin()+i);
					i--;
				}
			}
		} else {
			if (!physics[i]->length) {
				physics.erase(physics.begin()+i);
				i--;
			} else if (physics[i]->length > -1) physics[i]->length--;
		}
	}
}

void environment::roundInit()
{
	bg.x = bg.w/2 - screenWidth/2;
	bg.y = -screenHeight;
	while(physics.size() > 1)
		physics.pop_back();
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
	for(auto& i:physics){
		if(a->validate(i->ID, i->effectCode)) i->enforce(*a);
	}
}

void environment::enforceFloor(instance * a)
{
	/*Floor, or "Bottom corner"*/

	if (a->collision.y < floor){
		a->land();
		a->current.posY = floor - a->current.move->collision[a->current.frame].y;
		a->updateRects();
	}
}

void environment::enforceBounds(instance * a)
{
	if(a->current.rebound && a->current.posY + a->collision.h >= screenHeight){
		a->current.posY = screenHeight - a->collision.h;
		a->current.deltaY = -a->current.deltaY;
		a->current.rebound = false;
		a->updateRects();
	}
}

void environment::checkCorners(instance * a)
{
	int left = bg.x + wall,
	    right = bg.x + screenWidth - wall;
	/*Walls, or "Left and Right" corners
	This not only keeps the characters within the stage boundaries, but flags them as "in the corner"
	so we can specialcase a->collision checks for when one player is in the corner.*/

	/*Offset variables. I could do these calculations on the fly, but it's easier this way.
	Essentially, this represents the offset between the sprite and the a->collision box, since
	even though we're *checking* a->collision, we're still *moving* spr*/
	int lOffset = a->current.posX - a->collision.x,
	    rOffset = a->current.posX - (a->collision.x + a->collision.w);
	a->updateRects();
	if(a->collision.x <= left){
		a->encounterWall(0, wall);
		if(a->collision.x < left)
			a->current.posX = left + lOffset;
	} else a->current.lCorner = 0;
	if(a->collision.x + a->collision.w >= right){
		a->encounterWall(1, wall);
		if(a->collision.x + a->collision.w > right)
			a->current.posX = right + rOffset;
	} else a->current.rCorner = 0;
	a->updateRects(); //Update rectangles or the next a->collision check will be wrong.
}

