#include "analytics.h"
#include "player.h"
#include "tokenizer.h"
#include <SDL/SDL.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include "force.h"
using std::ifstream;
using std::ofstream;
using std::to_string;
using std::cout;

player::player()
{
	name = nullptr;
	init();
}

player::player(int id)
{
	ID = id;
	name = "";
	init();
	wins = 0;
}

bool player::validate(int id, int effect)
{
	return (id != ID) && (effect & 1);
}

bool instance::validate(int id, int effect)
{
	return (id != ID) && (effect & 2);
}

instance * instance::spawn()
{
	return pick()->spawn(current);
}

instance::instance()
{
	init();
}

instance::instance(avatar * f)
{
	v = f;
	current.meter = pick()->generateMeter();
	pick()->init(current);
	init();
}

hStat instance::pollStats()
{
	hStat s = pick()->pollStats(current);
	if(current.rCorner || current.lCorner){
		s.push *= 2;
	}
	return s;
}

int instance::comboState()
{
	current.comboState = pick()->comboState(current.move);
	return current.comboState;
}

int player::comboState()
{
	int t;
	if(current.move == pick()->down){
		t = pick()->comboState(save.move);
		switch(t){
		case -1:
		case 0:
			break;
		case 1: 
			current.comboState = 2;
			break;
		case -2:
			current.comboState = -3;
			break;
		}
		return current.comboState;
	} else return instance::comboState();
}

void instance::init()
{
	current.deltaX = 0;
	current.deltaY = 0;
	current.lCorner = 0;
	current.rCorner = 0;
	current.frame = 0;
	current.connect = 0;
	current.counter = 0;
	current.hit = 0;
	current.move = nullptr;
	current.bufferedMove = nullptr;
	current.reversal = nullptr;
	current.freeze = 0;
	current.aerial = false;
	current.dead = false;
	current.age = 0;
	current.throwInvuln = 0;
	current.offspring.clear();
	inputBuffer.clear();
	for(int i = 0; i < 30; i++) inputBuffer.push_front(5);
}

bool instance::acceptTarget(instance * m)
{
	if(m->ID == ID) return 0;
	if(m->current.move->hittable) return 1;
	return m->pick()->acceptTarget(current);
}

bool instance::checkHit(instance *o)
{
	if(current.move->hits > 0){
		for(SDL_Rect i:current.hitbox){
			for(SDL_Rect j:o->current.hitreg){
				SDL_Rect hitLoc;
				if(aux::checkCollision(i, j, hitLoc)){
					hitLoc.x -= current.posX;
					hitLoc.y -= current.posY;
					hitLocation.push_back(hitLoc);
					return true;
				}
			}
		}
	}
	return false;
}

void player::init()
{
	/*Initialize input containers*/
	inputName.push_back("Up");
	inputName.push_back("Down");
	inputName.push_back("Left");
	inputName.push_back("Right");
	inputName.push_back("A");
	inputName.push_back("B");
	inputName.push_back("C");
	inputName.push_back("D");
	inputName.push_back("E");
	inputName.push_back("Start");

	currentMacro = nullptr;
	record = nullptr;
	v = nullptr;
	rounds = 0;
	instance::init();
}

void player::roundInit()
{
	instance::init();
	neutralize();
	if(v) pick()->init(current);
	if(record){
		record->write(to_string(ID)+"-"+pick()->name+".sh");
		delete record;
		record = nullptr;
	}
	current.elasticX = 0;
	current.elasticY = 0;
	blockType = 0;
	iterator = 0;
	current.slide = 0;
	current.stick = 0;
	current.hover = 0;
	current.throwInvuln = 0;
	particleLife = 0;
	particleType = 0;
	search = 0;
	current.facing = ID == 1 ? 1 : -1;
	updateRects();
}

bool controller::readConfig(int ID)
{
	ifstream read;
	int i = 0;
	read.open(".config/p" + to_string(ID) + ".conf");
	if(read.fail()) {
		read.close();
		return 0;
	} else {
    int temp;
		do{
			input.push_back(new keySetting);
      read >> temp; 
      input[i]->trigger.type = temp;
			read.ignore(2);
			switch (input[i]->trigger.type){
			case SDL_JOYHATMOTION:
				read >> temp;
				input[i]->trigger.jhat.which = temp;
				read >> temp;
				input[i]->trigger.jhat.hat = temp;
				read >> temp;
				input[i]->trigger.jhat.value = temp;
				break;
			case SDL_JOYAXISMOTION:
				read >> temp;
				input[i]->trigger.jaxis.which = temp;
				read >> temp;
				input[i]->trigger.jaxis.axis = temp;
				read >> temp;
				input[i]->trigger.jaxis.value = temp;
				break;
			case SDL_JOYBUTTONDOWN:
        read >> temp; 
			  input[i]->trigger.jbutton.which = temp;
        read >> temp; 
				input[i]->trigger.jbutton.button = temp;
				break;
			case SDL_KEYDOWN:
        read >> temp;
				input[i]->trigger.key.keysym.sym = (SDLKey)temp;
				break;
			default:
				break;
			}
			read >> temp;
      input[i]->effect = temp;
			i++;
			read.peek();
		} while(!read.eof());
		read.close();
		return 1;
	}
}

/*This function wraps around the normal setKey function, handling input by itself*/
void player::setKey(int effect) 
{
	SDL_Event temp;
	bool configFlag = 0;

	while(SDL_PollEvent(&temp));
	while (!configFlag){
		if (SDL_PollEvent(&temp)) {
			configFlag = setKey(effect, temp);
		}
	}
}

/*This function takes an event and a desired effect and maps them in the keySetting array*/
bool player::setKey(int effect, SDL_Event temp)
{
	int workingIndex = -1;
	switch (temp.type){
	case SDL_JOYHATMOTION:
		if(temp.jhat.value != 0){
			for(unsigned int i = 0; i < input.size(); i++){
				if(input[i]->trigger.type == temp.type &&
				   input[i]->trigger.jhat.which == temp.jhat.which &&
				   input[i]->trigger.jhat.hat == temp.jhat.hat &&
				   input[i]->trigger.jhat.value == temp.jhat.value){
					workingIndex = i;
					i = input.size();
				}
			}
			if(workingIndex < 0){
				input.push_back(new keySetting);
				workingIndex = input.size() - 1;
				input[workingIndex]->trigger.type = temp.type;
				input[workingIndex]->trigger.jhat.which = temp.jhat.which;
				input[workingIndex]->trigger.jhat.hat = temp.jhat.hat;
				input[workingIndex]->trigger.jhat.value = temp.jhat.value;
			}
		}
		break;
	case SDL_JOYAXISMOTION:
		if(temp.jaxis.value != 0){
			for(unsigned int i = 0; i < input.size(); i++){
				if(input[i]->trigger.type == temp.type &&
				   input[i]->trigger.jaxis.which == temp.jaxis.which &&
				   input[i]->trigger.jaxis.axis == temp.jaxis.axis &&
				   input[i]->trigger.jaxis.value == temp.jaxis.value){
					workingIndex = i; 
					i = input.size();
				}
			}
			if(workingIndex < 0){
				input.push_back(new keySetting);
				workingIndex = input.size() - 1;
				input[workingIndex]->trigger.type = temp.type;
				input[workingIndex]->trigger.jaxis.which = temp.jaxis.which;
				input[workingIndex]->trigger.jaxis.axis = temp.jaxis.axis;
				input[workingIndex]->trigger.jaxis.value = temp.jaxis.value;
			}
		}
		break;
	case SDL_JOYBUTTONDOWN:
		for(unsigned int i = 0; i < input.size(); i++){
			if(input[i]->trigger.type == temp.type &&
			   input[i]->trigger.jbutton.which == temp.jbutton.which &&
			   input[i]->trigger.jbutton.button == temp.jbutton.button){
				workingIndex = i;
				i = input.size();
			}
		}
		if(workingIndex < 0){
			input.push_back(new keySetting);
			workingIndex = input.size() - 1;
			input[workingIndex]->trigger.type = temp.type;
			input[workingIndex]->trigger.jbutton.which = temp.jbutton.which;
			input[workingIndex]->trigger.jbutton.button = temp.jbutton.button;
		}
		break;
	case SDL_KEYDOWN:
		for(unsigned int i = 0; i < input.size(); i++){
			if(input[i]->trigger.type == temp.type &&
			   input[i]->trigger.key.keysym.sym == temp.key.keysym.sym){
				workingIndex = i;
				i = input.size();
			}
		}
		if(workingIndex < 0){
			input.push_back(new keySetting);
			workingIndex = input.size() - 1;
			input[workingIndex]->trigger.type = temp.type;
			input[workingIndex]->trigger.key.keysym.sym = temp.key.keysym.sym;
		}
		break;
	default:
		break;
	}
	if(workingIndex > -1){
		if(input[workingIndex]->effect & effect);
		else input[workingIndex]->effect += effect;
		return 1;
	} else return 0;
}

void controller::writeConfig(int ID)
{
	ofstream write;
	write.open(".config/p" + to_string(ID) + ".conf");
	for(unsigned int i = 0; i < input.size(); i++){
		switch(input[i]->trigger.type){
		case SDL_JOYHATMOTION:
			if(input[i]->trigger.jhat.value != 0){
				write << (int)input[i]->trigger.type << " : " << (int)input[i]->trigger.jhat.which << " " << (int)input[i]->trigger.jhat.hat << " " << (int)input[i]->trigger.jhat.value;
			}
			break;
		case SDL_JOYAXISMOTION:
			if(input[i]->trigger.jaxis.value != 0){
				write << (int)input[i]->trigger.type << " : " << (int)input[i]->trigger.jaxis.which << " " << (int)input[i]->trigger.jaxis.axis << " " << (int)input[i]->trigger.jaxis.value;
			}
			break;
		case SDL_JOYBUTTONDOWN:
			write << (int)input[i]->trigger.type << " : " << (int)input[i]->trigger.jbutton.which << " " << (int)input[i]->trigger.jbutton.button;
			break;
		case SDL_KEYDOWN:
			write << (int)input[i]->trigger.type << " : " << (int)input[i]->trigger.key.keysym.sym;
			break;
		}
		write << " " << input[i]->effect << "\n";
	}
	write.close();
}

bool player::reversalPossible() const
{
	if(current.cancelState() & 1) return false;
	if(current.move->linkable) return true;
	if(current.counter < 0 && current.counter > -11) return true;
	int f = current.move->frames;
	if(current.move->next) f += current.move->next->frames;
	if(current.frame + 10 < f) return false;
	if(current.frame < 4) return false;
	return true;
}

void player::enemySelect(int i)
{
	ifstream charlist;
	vector<string> chr (i+1);
	chr[0] = "White";
	charlist.open("src/charlist.h");
	int j = 0;
	while(j < i){
		char k;
		do charlist >> k;
		while(k != '/');
		do charlist >> k;
		while(k != '-');
		j++;
		charlist >> chr[j];
	}
	charlist.close();
	std::cout << name + "/" + chr[j] + name;
	pick()->avatar::build(name + "/" + chr[j], name);
}

void player::characterSelect(int i)
{
	v = nullptr;
	switch(i){
	case 2:
		v = new yellow;
		break;
	default:
		ifstream charlist;
		vector<string> chr (i+1);
		chr[0] = "White";
		charlist.open("src/charlist.h");
		int j = 0;
		while(j < i){
			char k;
			do charlist >> k;
			while(k != '/');
			do charlist >> k;
			while(k != '-');
			j++;
			charlist >> chr[j];
		}
		charlist.close();
		v = new character(chr[i]);
		break;
	}
	iterator = 0;
	current.meter = pick()->generateMeter();
	neutralize();
}

void player::readScripts()
{
	string buffer("");
	int tempIterator;
	macro.clear();
	ifstream read;
	read.open(".config/"+v->name+"rc");
	if(read.fail()) return;
	while(!read.eof()){
		tempIterator = 0;
		getline(read, buffer);
		tokenizer t(buffer, " \n");
		if(!t().size()) return;
		macro.push_back(new script(t.current()));
		if(t().size()){
			for(char c : t.current()){
				if(c >= 'A' && c <= 'E'){
					tempIterator += 16 << (c - 'A');
				}
			}
		}
		if(!macro[macro.size()-1]->test()) macro.pop_back();
		else pattern.push_back(tempIterator);
	}
}

void instance::updateRects()
{
	if(current.move != nullptr) {
		pick()->pollRects(current);
		for(unsigned int i = 0; i < current.hitbox.size(); i++){
			if(current.facing == -1) current.hitbox[i].x = current.posX - current.hitbox[i].x - current.hitbox[i].w;
			else current.hitbox[i].x += current.posX;
			current.hitbox[i].y += current.posY;
		}
		for(unsigned int i = 0; i < current.hitreg.size(); i++){
			if(current.facing == -1) current.hitreg[i].x = current.posX - current.hitreg[i].x - current.hitreg[i].w;
			else current.hitreg[i].x += current.posX;
			current.hitreg[i].y += current.posY;
		}
		if(current.facing == -1) current.collision.x = current.posX - current.collision.x - current.collision.w;
		else current.collision.x += current.posX;
		current.collision.y += current.posY;
	}
}

void instance::combineDelta()
{
	for(unsigned int i = 0; i < momentum.size(); i++){
		current.deltaX += momentum[i].x;
		current.deltaY += momentum[i].y;

		if(momentum[i].w <= 0) {
			momentum.erase(momentum.begin()+i);
			i--;
		}
		else momentum[i].w--;
	}
	if(current.hover > 0 && current.deltaY < 0) current.deltaY = 0;
	current.posX += current.deltaX;
	current.posY += current.deltaY;

	updateRects();
}

void instance::loadAssets()
{
	if(sprite) pick()->loadAssets(selectedPalette);
}

void instance::checkReversal()
{
	if(current.move){
		if(current.reversal){
			if(*current.reversal > current){
				checkFacing();
				current.move = current.reversal->execute(current);
				current.reversalFlag = true;
				current.reversal = nullptr;
				current.reversalTimer = 0;
			}
		}
	} else neutralize();
}

void player::checkBlocking()
{
	if(current.cancelState() & 513){
		blockType = -pick()->checkBlocking(current, inputBuffer);
		if(blockType == 1) blockType = !current.move->canGuard(current.frame);
		updateRects();
	}
}

bool instance::stuck()
{
	return (current.counter < 0);
}

bool player::stuck()
{
	return (current.move == pick()->untech || current.move == pick()->die);
}

void instance::encounterWall(bool side, int wallPosition)
{
	if(side) {
		if(current.elasticX){
			current.rCorner = 0;
			if(current.deltaX > 0) current.deltaX = -current.deltaX; 
			current.elasticX = false;
		}
		if(current.collision.x + current.collision.w >= wallPosition){ //TODO: This is obviously kludgey to hardcode
			if(current.facing == -1) current.rCorner = 1;
			else {
				current.posX--;
			}
			if (current.stick) {
				if(stuck()){
					current.deltaX = 0;
					current.deltaY = 0;
					momentum.clear();
				} else current.stick = 0;
			}
		}
	} else {
		if(current.elasticX){
			current.lCorner = 0;
			if(current.deltaX < 0) current.deltaX = -current.deltaX;
			current.elasticX = false;
		}
		if(current.collision.x <= wallPosition){ //TODO: This is obviously kludgey to hardcode
			if(current.facing == 1) current.lCorner = 1;
			else current.posX++;
			if (current.stick) {
				if(stuck()){
					current.deltaX = 0;
					current.deltaY = 0;
					momentum.clear();
				} else current.stick = 0;
			}
		}
	}
}

void player::land()
{
	instance::land();
	if(current.move == pick()->down){
		for(instance *i:current.offspring) i->passSignal(-3);
	}
}

void instance::land()
{
	if(current.elasticY){
		current.deltaY = -current.deltaY;
		current.elasticY = false;
	} else if (current.slide) {
		current.deltaY = 0;
		if(stuck()){ 
			if(current.deltaX < 0) current.deltaX++;
			else if(current.deltaX > 0) current.deltaX--;
			current.aerial = 1;
		} else {
			current.deltaX = 0;
			current.slide = 0;
		}
	} else {
		if(current.aerial == 1){
			for(unsigned int i = 0; i < momentum.size(); i++){
				if(momentum[i].y > 0) momentum.erase(momentum.begin()+i);
			}
			current.aerial = false;
			pick()->land(current);
			current.reversal = nullptr;
			updateRects();
			current.deltaX = 0;
		}
		current.deltaY = 0;
	}
}

void instance::checkFacing()
{
	checkFacing(current.opponent);
}

void instance::follow()
{
	follow(current.opponent);
}

void instance::follow(instance *other){
	if(current.frame >= current.move->followStart && current.frame <= current.move->followEnd){
		if(abs(other->current.posX - current.posX) > current.move->followXRate){ 
			if(other->current.posX > current.posX) current.posX += current.move->followXRate;
			else if(other->current.posX < current.posX) current.posX -= current.move->followXRate;
		} else current.posX += other->current.posX - current.posX;
		if(abs(other->current.posY - current.posY) > current.move->followYRate){ 
			if(other->current.posY > current.posY) current.posY += current.move->followYRate;
			else if(other->current.posY < current.posY) current.posY -= current.move->followYRate;
		} else current.posY += other->current.posY - current.posY;
	}
}

void instance::print()
{
	cout << "Player" << ID << "(" << pick()->name << "): " << current.move->name << "[" << current.frame << "]\n";
}

bool instance::isDead()
{
	return pick()->isDead(current);
}

bool player::isDead()
{
	return current.meter[0].value == 0 || pick()->isDead(current);
}

void instance::manageOffspring()
{
	for(unsigned int i = 0; i < current.offspring.size(); i++){
		if(current.offspring[i]->current.move == current.offspring[i]->pick()->die){
			current.offspring.erase(current.offspring.begin()+i);
			i--;
		} else if (current.offspring[i]->ID != ID) {
			if(current.offspring[i]->ID == current.opponent->ID)
				current.opponent->current.offspring.push_back(current.offspring[i]);
			current.offspring.erase(current.offspring.begin()+i);
			i--;
		}
	}
}

void instance::step()
{
	if(!current.freeze){
		if(current.throwInvuln > 0) current.throwInvuln--;
		current.hover--;
	}
	if(pick()->isDead(current)) current.dead = true;
	manageOffspring();
	if(current.connect < 0) current.connect = 0;
	if(!current.freeze){ 
		if(current.move->flip == current.frame) flip();
		current.age++;
	}
	pick()->step(current);
	if(current.move && current.frame >= current.move->frames){
		if(current.move->modifier && current.move->basis.move){ 
			current.frame = current.move->basis.frame;
			current.connect = current.move->basis.connect;
			current.hit = current.move->basis.hit;
			current.move = current.move->basis.move;
		} else {
			if(current.move->next) current.move = current.move->next->execute(current);
			else neutralize();
		}
	}
	if(current.reversalTimer)
		current.reversalTimer--;
	else
		current.reversal = nullptr;
}

void instance::neutralize()
{
	current.move = pick()->neutralize(current);
}

hStat player::generateParry (hStat &a)
{
	hStat ret;
	ret.damage = a.chip ? a.chip : a.damage/5;
	ret.ghostHit = true;
	ret.stun = 0;
	ret.push = a.push;
	if(current.aerial) ret.push += (current.opponent->current.aerial) ? a.blowback : a.blowback*5;
	ret.pause = 0;
	return ret;
}


void instance::flip()
{
	if(current.facing == -1){
		current.posX += current.collision.x - (current.posX + (current.posX - current.collision.x - current.collision.w));
		current.facing = 1;
	} else { 
		current.posX += (current.collision.w + current.collision.x) - current.posX*2 + current.collision.x;
		current.facing = -1;
	}
}

void instance::checkFacing(instance * other)
{
	int comparison, midpoint;
	midpoint = current.collision.x + current.collision.w/2;
	comparison = other->current.collision.x + other->current.collision.w/2;

	if(other->current.posX < current.posX) comparison += current.collision.w % 2; 
	else midpoint += current.collision.w % 2;

	if (current.lCorner || other->current.rCorner) current.facing = 1;
	else if (current.rCorner || other->current.lCorner) current.facing = -1;
	else if (midpoint < comparison){
		if(current.facing == -1) flip();
	} else if (midpoint > comparison){
		if(current.facing == 1) flip();
	}
}

int instance::dragBG(int left, int right)
{
	if(current.collision.x < left) return current.collision.x - left;
	else if (current.collision.x + current.collision.w > right) return (current.collision.x + current.collision.w) - right;
	else return 0;
}

void instance::passSignal(int sig)
{
	pick()->signal(sig, current);
}

void instance::pushInput(deque <int> i)
{
	inputBuffer = i;
}

void instance::pushInput(unsigned int i)
{
	inputBuffer.push_front(i);
	inputBuffer.pop_back();
}

void instance::cleanup()
{
	save = current;
}

void instance::getMove(vector<int> buttons)
{
	checkReversal();
	status e = current;
	int n = current.frame;
	pick()->prepHooks(current, inputBuffer, buttons);
	if(current.move){
		if(current.move->throwinvuln == 1 && current.throwInvuln <= 0) current.throwInvuln = 1;
		if(current.move->throwinvuln == 2) current.throwInvuln = 6;
	}
	if(current.frame != n || current.move != e.move) current.move->playSound(ID);
}

void instance::pullVolition()
{
	int top = 0;
	for(unsigned int i = 0; i < momentum.size(); i++)
		if(momentum[i].h > 0 && momentum[i].h > top){ 
			top = (short)momentum[i].h;
		}
	if(current.move->stop){
		if(current.frame == 0){
			if(current.move->stop & 1){ 
				current.deltaX = 0; current.deltaY = 0;
			}
			if(current.move->stop & 2)
				momentum.clear();
		}
	}
	int dx = current.move->displace(current.posX, current.posY, current.frame);
	setPosition(current.posX + current.facing*dx, current.posY);
	if(current.freeze < 1){
		if(current.frame < current.move->frames){
			vector<SDL_Rect> temp = current.move->pollDelta(current.frame);
			for(unsigned int i = 0; i < temp.size(); i++){
				temp[i].x *= current.facing;
				if(temp[i].x || temp[i].y || temp[i].h){
					if(abs((short)temp[i].h) >= top || top == 0){
						momentum.push_back(temp[i]);
					}
				}
			}
		}
	}
}

int instance::middle()
{
	if(current.facing == 1) return current.collision.x + current.collision.w / 2;
	else return current.collision.x + current.collision.w / 2 + current.collision.w % 2;
}

void player::macroCheck(SDL_Event &event)
{
	int effect = tap(event);
	if(effect > 0){
		currentMacro = nullptr;
		if(effect & 512) search = true;
	} else if (effect < 0) {
		if(abs(effect) & 512){
			search = false;
			if(!currentMacro){
				if(!record){
					record = new script();
					record->init(1);
				} else {
					record->write(to_string(ID) + "-" + v->name + ".sh");
					delete record;
					record = nullptr;
				}
			}
		}
	}
}

void controller::readEvent(SDL_Event & event, frame &t)
{
	int effect = tap(event);
	if(effect != 0){
		for(int i = 0; i < 4; i++){
			if(abs(effect) & (1 << i)){
				if(effect > 0){
					t.axis[i] = 1;
				} else {
					t.axis[i] = 0;
				}
				if(i%2 == 0) t.axis[i+1] = 0;
				else t.axis[i-1] = 0;
			}
		}
		for(unsigned int i = 0; i < t.buttons.size(); i++){
			if(abs(effect) & (1 << (i + 4))){
				if(effect > 0) t.buttons[i] = 1;
				else if(effect < 0) t.buttons[i] = -1;
			}
		}
	}
}

void player::readEvent(SDL_Event & event, frame &t)
{
	if(event.type == SDL_JOYHATMOTION){
		for(unsigned int i = 0; i < input.size(); i++){
			if(input[i]->trigger.jhat.hat == event.jhat.hat &&
			   input[i]->trigger.jhat.which == event.jhat.which &&
			   input[i]->trigger.jhat.value > 0){
				if(event.jhat.value & 1) t.axis[0] = true;
				else if(event.jhat.value & 4) t.axis[1] = true;
				else {
					t.axis[0] = false;
					t.axis[1] = false;
				}
				if(event.jhat.value & 8) t.axis[2] = true;
				else if(event.jhat.value & 2) t.axis[3] = true;
				else {
					t.axis[2] = false;
					t.axis[3] = false;
				}
				break;
			}
		}
	} else {
		controller::readEvent(event, t);
	}
	unsigned int d = 5;
	if(t.axis[0]) d += 3;
	if(t.axis[1]) d -= 3;
	if(t.axis[2]) d--;
	if(t.axis[3]) d++;
	if(t.buttons[5] == 1) t.n.raw.Start = true;
	else t.n.raw.Start = false;
	t.n.raw.dir = d;
	//t.n.raw.Player = ID%2;
}

void instance::connect(hStat & s)
{
	if(s.pause < 0){
		current.freeze = s.ghostHit ? 0 : s.stun/4+10;
	} else current.freeze = s.pause;
	if(s.autoCorrects) checkFacing();
	else if(!current.aerial){
		for(int i = 0; i < 6; i++){
			if(2 << i & current.cancelState()){
				checkFacing();
				break;
			}
		}
	}
	for(instance *i:current.offspring) i->passSignal(2);
	pick()->connect(current);
	current.reversal = nullptr;
	if(current.bufferedMove == current.move) current.bufferedMove = nullptr;
}

int instance::takeHit(hStat & s)
{
	if(s.turnsProjectile){
		if(pick()->turn(ID)){ 
			flip();
			invertVectors(1);
			current.deltaX = -current.deltaX;
			current.freeze = 0;
		}
	}
	current.reversal = nullptr;
	return pick()->takeHit(current, s, blockType, particleType);
}

int player::takeHit(hStat & s)
{
	SDL_Rect v = {0, 0, 1, 0};
	action * counterAttack = nullptr;
	current.reversal = nullptr;
	int f;
	if(current.slide) s.lift += 15 - abs(s.lift)/4;
	f = instance::takeHit(s);
	current.freeze = f;
	if(particleType != 1){
		counterAttack = current.move->blockSuccess(s.stun, s.isProjectile);
	}
	if(counterAttack && counterAttack != current.move && counterAttack->check(current)){
		current.bufferedMove = counterAttack;
		current.freeze = 0;
	} else {
		particleLife = 8;
		current.deltaX /= 6;
		if(current.deltaY < 0) current.deltaY /= 55;
		else current.deltaY /= 6;
		momentum.clear();
		if(current.aerial) v.y = s.lift;
		else v.y = 0;
		if(current.aerial) v.x = -(s.push/5 + s.blowback);
		else v.x = -s.push;
		v.x *= current.facing;
		if(particleType == -1){ 
			v.x /= 5;
			v.y /= 5;
		}
		if(particleType <= -2){
			v.x = 0;
			v.y = 0;
			current.freeze = 0;
			current.meter[4] = 0;
		}
		momentum.push_back(v);
		if(current.aerial && s.hover){
			current.hover = s.hover;
		}
		else current.hover = 0;
		if(current.aerial && s.wallBounce) current.elasticX = true;
		else current.elasticX = false;
		if(current.aerial && s.floorBounce) current.elasticY = true;
		else current.elasticY = false;
		if(current.aerial && s.slide) current.slide = true;
		else current.slide = false;
		if(s.ceilingBounce) current.rebound = true;
		if(current.aerial && s.stick) current.stick = true;
		else current.stick = false;
	}
	if(current.move == pick()->die){
		current.bufferedMove = nullptr;
	}
	for(instance *i:current.offspring) i->passSignal(particleType);
	updateRects();
	if(s.ghostHit) return 0;
	else if(particleType == 1) return particleType;
	else {
		return -1;
	}
}

void instance::outOfBounds()
{
	current.move = pick()->die->execute(current);
}

void instance::invertVectors(int operation)
{
	switch (operation){
	case 1:
		for(unsigned int i = 0; i < momentum.size(); i++)
			momentum[i].x = -momentum[i].x;
		break;
	case 2:
		for(unsigned int i = 0; i < momentum.size(); i++)
			momentum[i].y = -momentum[i].y;
		break;
	case 3:
		for(unsigned int i = 0; i < momentum.size(); i++){
			momentum[i].x = -momentum[i].x;
			momentum[i].y = -momentum[i].y;
		}
		break;
	default:
		return;
		break;
	}
}

int player::CHState() const
{
	if(!current.hitbox.empty()) return true;
	else return current.move->CHState(current.frame);
}

void instance::setPosition(int x, int y)
{
	current.posX = x;
	current.posY = y;
	updateRects();
}

void player::getThrown(action *toss, int x, int y)
{
	int xSign = x / abs(x);
	updateRects();
	momentum.clear();
	current.deltaX = 0;
	current.deltaY = 0;
	hStat dummy;
	dummy.stun = 1;
	dummy.ghostHit = 1;
	setPosition(toss->arbitraryPoll(27, current.frame)*xSign + abs(x), toss->arbitraryPoll(26, current.frame) + y);
	neutralize();
	pick()->takeHit(current, dummy, 0, particleType);
	current.counter = -5;
}

instance::~instance(){}

player::~player(){}

