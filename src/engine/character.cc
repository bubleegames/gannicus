#include "character.h"
#include <assert.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>

using std::to_string;
using std::cout;
using std::ifstream;

character::character()
//Character constructor. This loads the whole character into memory so that that we don't have disk reads during gameplay
{
	//build("genericCharacter", "genericCharacter");
}

character::character(string name)
{
	head = new actionTrie;
	airHead = new actionTrie;
	build(name, name);
}

bool character::isDead(status &current)
{
	return current.move == dead;
}

character::~character()
{
	if(!dFlag){
		if(head) delete head;
		if(airHead) delete airHead;
		delete neutral;
		delete crouch;
		delete reel;
		delete untech;
		delete crouchReel;
		delete crouchBlock;
		delete standBlock;
		delete airBlock;
		delete down;
	}
}

void avatar::getReversal(status &current, deque<int> inputBuffer, vector<int> buttons)
{
	status temp = current;
	temp.move = neutralize(temp);
	if(temp.bufferedMove) temp.move = temp.bufferedMove;
	temp.reversalFlag = current.reversalFlag;
	temp.connect = 0;
	temp.hit = 0;
	if (!current.reversal && (current.move->linkable || current.frame > 4 || current.counter < 0)){
		current.reversal = hook(temp, inputBuffer, buttons);
		if(current.reversal){
			if(current.reversal->state[0].b.neutral){
				current.reversal = nullptr;
				current.reversalTimer = 0;
			}
		}
		if(current.move->linkable) {
			current.reversalTimer = -1;
		} else {
			bool b = false;
			for(int i:buttons) if(i) b = true;
			current.reversalTimer = 5 + b*6;
		}
	}
}

void avatar::prepHooks(status &current, deque<int> inputBuffer, vector<int> buttons)
{
	action * ret = nullptr;
	ret = hook(current, inputBuffer, buttons);
	if(!ret){
		if(current.move->followupSpan.end && current.move->followupSpan == current.frame);
		if(current.move->window(current.frame)){
			if(current.move->attempt->check(current)){
				ret = current.move->attempt;
			}
		} else if(current.move->holdFrame == current.frame){
			if(current.move->onHold->patternMatch(buttons, current.move->holdCheck, 0, 0) && current.move->onHold->check(current)){
				ret = current.move->onHold;
			}
		}
		if (current.bufferedMove && current.freeze <= 0){ 
			ret = current.bufferedMove;
			current.bufferedMove = nullptr;
		} else getReversal(current, inputBuffer, buttons);
	}
	bool triggerDelay = false;
	if (current.move->delayFrame == current.frame && current.delay < current.move->delayMax){
		for(unsigned int i = 0; i < buttons.size(); i++){
			if(current.move->delayCheck & (1 << i)){
				if(buttons[i] > 0){
					triggerDelay = true;
				}
			}
		}
	}
	current.delay = triggerDelay ? current.delay + 1 : 0;
	if(ret){
		current.reversalFlag = false;
		if(!ret->freezeAgnostic && current.freeze > 0){
			if(current.bufferedMove == nullptr){
				current.bufferedMove = ret;
			}
		} else {
			current.move = ret->execute(current);
			current.bufferedMove = nullptr;
		}
	}
}

action * avatar::hook(status &current, deque<int> inputBuffer, vector<int> buttons)
{
	return head->actionHook(current, inputBuffer, 0, -1, buttons);
}

action * character::hook(status &current, deque<int> inputBuffer, vector<int> buttons)
{
	if(current.aerial) return airHead->actionHook(current, inputBuffer, 0, -1, buttons);
	else return avatar::hook(current, inputBuffer, buttons);
}

void avatar::signal(int sig, status &current)
{
	switch(sig){
	case 1:
		if(killFlags & 1) current.move = die->execute(current);
		break;
	case 0:
		if(killFlags & 2) current.move = die->execute(current);
		break;
	case -1:
		if(killFlags & 4) current.move = die->execute(current);
		break;
	case -2:
		if(killFlags & 8) current.move = die->execute(current);
		break;
	case -3:
		if(killFlags & 16) current.move = die->execute(current);
		break;
	}
}

action * avatar::neutralize(status &current)
{
	current.reversalFlag = false;
	return neutral->execute(current);
}

action * character::neutralize(status &current)
{
	current.reversalFlag = false;
	if(current.aerial){ 
		return airNeutral->execute(current);
	} else {
		return neutral->execute(current);
	}
}

void avatar::getName(string directory, string file)
{
	string buffer = "content/characters/" + directory + "/" + file + ".ch";
	ifstream read;

	read.open(buffer);
	if(read.fail()) getName("template", "template");

	read >> name;
}

int avatar::comboState(action * c)
{
	return 0;
}

int character::comboState(action * c)
{
	if(c == reel) return 1;
	if(c == crouchReel) return 1;
	if(c == untech) return 1;
	if(c == standBlock) return -1;
	if(c == crouchBlock) return -1;
	if(c == airBlock) return -1;
	if(c == fall) return -2;
	return 0;
}

action * avatar::dealWithMove(string input)
{
	if(input[0] == '#') return nullptr;
	bool replace = false;
	action * m = createMove(input);
	for(unsigned int i = 0; i < moveList.size(); i++){
		if(*moveList[i] == m->fileName){
			if(m->null){
				moveList[i]->allowed.i = 0;
				//cout << "Disabling ";
			} else {
				char k = m->typeKey;
				moveList[i]->build(name, m->fileName);
				m->typeKey = k;
				processMove(moveList[i]);
				sortMove(moveList[i], input);
				//cout << "Replacing with " << name + "/";
			}
			replace = true;
			//cout << m->fileName;
			delete m;
			m = moveList[i];
			break;
		}
	}
	if(!replace){
		//cout << input + " (No match)";
		if(m->null) delete m;
		else{
			//cout << "New move " << name + "/" + m->fileName + " created\n";
			moveList.push_back(m);
			processMove(m);
			sortMove(m, input);
		}
	}
	return m;
}

void avatar::build(string directory, string file)
{
	lifespan = -1;
	captures = 0;
	killFlags = 0;
	char buffer[101];
	ifstream read;
	read.open("content/characters/"+directory+"/"+file+".ch");
	dir = directory;

	if(read.fail()){ 
		cout << directory << "/" << file << " Character definition not found\n";
		return;
	}

	read.get(buffer, 50); read.ignore(100, '\n');

	while(!read.eof()){
		read.get(buffer, 101, '\n'); read.ignore(100, '\n');
		dealWithMove(buffer);
	}
	read.close();
}

void avatar::sortMove(action * m, string key)
{
	bool initialized = false;
	tokenizer t(key, " \t=~>-&?@%$_!\n");
	t();
	while (t().size()){
		if(!initialized){
			head->remove(m);
			initialized = true;
		}
		switch (t.current()[0]){
		case 'h':
			head->insert(m, t.current());
			break;
		default:
			break;
		}
	}
}

void avatar::loadAssets(int pal)
{
	string p = "content/characters/"+dir+"/palette" + to_string(pal) + ".png";
	palette = aux::load_texture(p);
	for(action *i:moveList){
		if(i->payload) i->payload->loadAssets(pal);
		i->loadMisc(name);
	}
}

void character::sortMove(action * m, string key)
{
	bool initialized = false;
//	bool first = true;
	if(m->null) return;
	tokenizer t(key, " \t=>-&?@%$_!\n");
        t();
	while (t().size()){
		if(!initialized){
			head->remove(m);
			airHead->remove(m);
			initialized = true;
		}
//		if(first) first == false;
//		else CLEntry += " OR ";
		switch (t.current()[0]){
		case 'h':
			head->insert(m, t.current());
			break;
		case 'a':
			airHead->insert(m, t.current());
			break;
		default:
			break;
		}
	}
}


action * avatar::mandateMove(string key)
{
	action * m = createMove(key);
	if(m->null){ 
		delete m;
		string temp = name;
		name = "template";
		m = createMove(key);
		name = temp;
	}
	assert(!m->null); /*Hopefully this will end up the ONLY assert.
						It's a well-warranted one though, since if you
						don't have these moves, you're gonna have a bad 
						time
						*/
	moveList.push_back(m);
	processMove(m);
	return m;
}

void character::build(string directory, string file)
{
	getName(directory, file);

	neutral = mandateMove("@NS");
	dFlag = 0;

	string temp = name;
	name = "template";
	avatar::build("template", "template");
	name = temp;
	avatar::build(directory, file);

	die = mandateMove("`die");
	dead = mandateMove("@dead");
	die->feed(dead, 0, 0);
	die->feed(dead, 1, 0);

	crouch = mandateMove("@NL");

	airNeutral = mandateMove("@jNS");
	airNeutral->feed(neutral, 1, 0);

	reel = mandateMove(",HS");
	fall = mandateMove("@jfall");
	untech = mandateMove("`UT");
	untech->feed(fall, 0, 0);

	down = mandateMove("-down");
	untech->feed(down, 1, 0);
	fall->feed(down, 1, 0);

	crouchReel = mandateMove(",HL");

	standBlock = mandateMove(",BH");
	crouchBlock = mandateMove(",BL");
	airBlock = mandateMove(",BA");
	throwBreak = mandateMove("break");

	head->insert(5, neutral);

	head->insert(2, crouch);
	head->insert(3, crouch);
	head->insert(1, crouch);

	airHead->insert(5, airNeutral);
}

void avatar::processMove(action * m)
{
	if(m->null) return;
	string temp;
	action* t = nullptr;
	for(int i = 0; i < 7; i++){
		if(i == 2){
			for(int j = 0; j < m->hits; j++){
				temp = m->request(i, j);
				if(!temp.empty()){
					t = dealWithMove(temp);
					m->feed(t, i, j);
				}
			}
		} else if (i == 4) {
			temp = m->request(i, 0);
			if(!temp.empty()){
				m->generate(name.c_str(), temp.c_str());
			}
		} else {
			temp = m->request(i, 0);
			if(!temp.empty()){
				t = dealWithMove(temp);
				m->feed(t, i, 0);
			}
		}
	}
}

action * avatar::createMove(string key)
{
	tokenizer t(key, " \t=`~/>-&,?@%$+_!^\n");
	string token = t();
	action * m;
	switch(key[0]){
	case '%':
		if(key[1] == 'j') m = new airSpecial(name, token);
		else m = new special(name, token);
		break;
	case '~':
		if(key[1] == 'j') m = new airNegNormal(name, token);
		else m = new negNormal(name, token);
		break;
	case '-':
		if(key[1] == 'j') m = new airUtility(name, token);
		else m = new utility(name, token);
		break;
	case '@':
		if(key[1] == 'j') m = new airLooping(name, token);
		else m = new looping(name, token);
		break;
	case '^':
		if(key[1] == 'j') m = new airReleaseCheck(name, token);
		else m = new releaseCheck(name, token);
		break;
	case '?':
		m = new mash(name, token);
		break;
	case '`':
		m = new untechState(name, token);
		break;
	case ',':
		m = new hitstun(name, token);
		break;
	case 'j':
		m = new airMove(name, token);
		break;
	default:
		m = new action(name, token);
		m->typeKey = 'n';
		break;
	}
	if(m->typeKey == '0') m->typeKey = key[0];
	return m;
}

instance * avatar::spawn(status &current)
{
	instance * n = current.move->spawn();
	if(!n) return nullptr;
	else {
		current.offspring.push_back(n);
		return current.offspring.back();
	}
}

void avatar::connect(status &current)
{
	action * t = current.move->connect(current);
	if(t != nullptr){
		current.bufferedMove = t;
	}
}

int character::checkBlocking(status &current, deque<int> inputBuffer)
{
	int st;
	if(current.hit < 0) current.hit = 0;
	if(current.connect < 0) current.connect = 0;
	bool success = false;
	int ret = -1;
	st = -(current.counter);
	if(st <= 0) st = 0;
	switch(inputBuffer[0]){
	case 3:
	case 6:
	case 9:
		for(int i = 1; i < 4; i++){
			if(inputBuffer[i] % 3 == 2){
				ret = 2;
				block(current, st, (inputBuffer[0] > 3));
				break;
			}
		}
		break;
	case 7:
	case 4:
	case 1:
		block(current, st, (inputBuffer[0] > 3));
		success = true;
		break;
	}
	if(success){
		ret = 0;
		for(int i = 1; i < 7; i++){
			if(inputBuffer[i] % 3 != 1)
				ret = 1;
		}
	}
	return ret;
}

void character::block(status &current, int st, bool high)
{
	resetAirOptions(current.meter);
	current.frame = 0;
	current.hit = 0;
	current.connect = 0;
	current.counter = -st;
	if(current.aerial){
		if(*airBlock > current) {
			current.move = airBlock->execute(current);
		}
	} else {
		if(high){
			if(*standBlock > current) {
				current.move = standBlock->execute(current);
			}
		} else {
			if(*crouchBlock > current) {
				current.move = crouchBlock->execute(current);
			}
		}
	}
}

void avatar::pollRects(status& current)
{
	current.move->pollRects(current, current.collision, current.hitreg, current.hitbox);
}

hStat avatar::pollStats(status &current)
{
	hStat s = current.move->pollStats(current.frame, current.counter);
	s.isProjectile = false;
	return s;
}

int character::assessStun(status &current, hStat &s)
{
	int stun = 0;
	if(!current.move->armor(current)){
		stun -= s.stun;
		if(current.aerial){
			stun -= s.untech;
			current.move = untech->execute(current);
			resetAirOptions(current.meter);
		} else {
			if(s.launch){
				stun -= s.stun + s.untech + s.initialLaunch;
				current.bufferedMove = untech;
				current.aerial = true;
			} else if(current.move->crouch)
				stun -= s.stun/5;

			if((!s.forceStand && current.move->crouch) || s.forceCrouch) {
				current.move = crouchReel->execute(current);
			} else {
				current.move = reel->execute(current);
			}
		}
	}
	return stun;
}

int character::takeHit(status &current, hStat &s, int blockType, int &hitType)
{
	bool dead = false;
	int freeze = 0;
	if(s.pause < 0){
		freeze = s.stun/4 + 10;
		if(s.ghostHit) freeze = 0;
	} else freeze = s.pause;
	current.absorbedHits++;
	hitType = current.move->takeHit(s, blockType, current);

	if(hitType == 1) current.meter[0].value -= s.damage;
	else if(hitType > -2) {
		current.meter[0].value -= s.chip;
		if(hitType == -1 && current.meter[0].value <= 0){ 
			current.meter[0].value = 1;
		}
	}
	if(current.meter[0].value <= 0){
		current.meter[0].value = 0;
		dead = true;
	}
	if(dead == true){
		current.counter = -(s.stun+s.untech);
		current.move = die->execute(current);
		current.aerial = true;
	} else if(hitType == 1) {
		if(s.stun != 0){
			current.counter = assessStun(current, s);
			if(current.counter < 0){
				current.frame = 0;
				current.hit = 0;
				current.connect = 0;
			}
		}
	} else if (hitType == -1) {
		if(!current.meter[4].value){
			if(current.meter[1].value + 6 < 300) current.meter[1].value += 12;
			else current.meter[1].value = 300;
		}
	}

	if (hitType == 1){ 
		if(!current.meter[4].value){
			current.meter[1].value += 2;
		}
	} else if (hitType > -2) {
		if(!current.meter[4].value){
			if(current.meter[1].value + 1 < 300) current.meter[1].value += 3;
			else current.meter[1].value = 300;
		}
	}
	return freeze;
}

vector<HUDMeter<int>> avatar::generateMeter()
{
	vector<HUDMeter<int>> meter;
	meter.push_back(HUDMeter<int>(600));
	meter.push_back(HUDMeter<int>(300));
	meter.push_back(HUDMeter<int>(1));
	meter.push_back(HUDMeter<int>(1));
	meter.push_back(HUDMeter<int>(600));

	meter[0].y = 1.0/90.0;
	meter[0].x = 1.0/8.0;
	meter[0].h = 1.0/90.0;
	meter[0].w = 1.0/8.0 * 6.0;
	meter[0].R = 1.0; meter[0].G = 0.0; meter[0].B = 0.0; meter[0].A = 1.0;

	meter[1].y = 1.0/90.0 * 87;
	meter[1].x = 1.0/8.0;
	meter[1].h = 1.0/90.0;
	meter[1].w = 1.0/8.0 * 6.0;
	meter[1].A = 1.0;

	meter[4].y = 1.0/90.0 * 87;
	meter[4].h = 1.0/90.0;
	meter[4].w = 1.0/8.0 * 6.0;
	meter[4].A = .4;

	meter[2].w = 0;
	meter[3].w = 0;
	return meter;
}

void character::init(status& current)
{
	current.meter[0].value = 600;
	current.meter[1].value = 0;
	resetAirOptions(current.meter);
	current.meter[4].value = 0;
	current.mode = 0;
}

void character::resetAirOptions(vector<HUDMeter<int>>& meter)
{
	meter[2].value = meter[2].maximum;
	meter[3].value = meter[3].maximum;
}

int avatar::acceptTarget(status &)
{
	return 1;
}

void avatar::land(status& current)
{
	current.move = current.move->land(current);
	if(!current.move){
		current.move = neutralize(current);
	}
}

void character::land(status& current)
{
	if(current.move->stunCheck(current)){
		current.move = standBlock;
	} else {
		avatar::land(current);
	}
	resetAirOptions(current.meter);
}

void avatar::step(status &current)
{
	for(unsigned int i = 0; i < current.cooldowns.size(); i++){
		if(current.cooldowns[i].duration > -1){
			if(!current.cooldowns[i].duration){ 
				current.cooldowns.erase(current.cooldowns.begin()+i);
				i--;
			} else current.cooldowns[i].duration--;
		}
	}

	if(current.freeze <= 0){
		current.move->step(current);
		tick(current);
		if(current.counter > 0) current.counter = 0;
	} else {
		current.freeze--;
	}
	if(current.meter[4].value > 0) current.meter[4].value--;
}
