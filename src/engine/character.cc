#include "character.h"
#include <assert.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>

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

void avatar::prepHooks(status &current, int inputBuffer[30], vector<int> buttons, SDL_Rect &p, bool dryrun, vector<int> & meter)
{
	action * t = nullptr;
	if(current.move && current.reversal){
		if(*current.reversal > current){
			if(current.reversal->check(p, meter)){
				if(!dryrun) current.reversal->execute(neutral, current, meter);
				current.move = current.reversal;
				current.reversalFlag = true;
				if(!dryrun){
					current.reversal = nullptr;
					current.reversalTimer = 0;
				}
			}
		}
	}
	else if(!current.move) neutralize(current, current.move, meter);
	t = hook(inputBuffer, 0, -1, meter, buttons, current.move, p, current.connect, current.hit, current.aerial);
	if(t == nullptr){
		if(current.move->window(current.frame)){
			if(current.move->attempt->check(p, meter)){
				t = current.move->attempt;
			}
		}
		else if(current.move->holdFrame == current.frame){
			if(current.move->onHold->activate(buttons, current.move->holdCheck, 0, 0, meter, p)){
				t = current.move->onHold;
			}
		}
		if (current.bufferedMove != nullptr && current.freeze <= 0) {
			if(!dryrun){ 
				current.bufferedMove->execute(current.move, current, meter);
			}
			current.move = current.bufferedMove;
			if(!dryrun) current.bufferedMove = nullptr;
		} else {
			action * r;
			bool l = current.reversalFlag;
			neutralize(current, r, meter);
			current.reversalFlag = l;
			if (!current.reversal && (current.move->linkable || current.frame > 4 || current.counter < 0)){
				int l = 0, m = 0;
				current.reversal = hook(inputBuffer, 0, -1, meter, buttons, r, p, l, m, current.aerial);
				if(current.reversal){
					if(current.reversal->state[0].b.neutral) 
						current.reversal = nullptr;
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
	}
	if(t != nullptr) {
		current.reversalFlag = false;
		if(current.freeze > 0){
			if(current.bufferedMove == nullptr){ 
				if(!dryrun) current.bufferedMove = t;
			}
		} else {
			if(!dryrun) t->execute(current.move, current, meter);
			current.move = t;
		}
	}
}

action * avatar::hook(int inputBuffer[30], int i, int f, vector<int> meter, vector<int> buttons, action * c, SDL_Rect &p, int &cFlag, int &hFlag, bool aerial)
{
	return head->actionHook(inputBuffer, 0, -1, meter, buttons, c, p, cFlag, hFlag);
}

action * character::hook(int inputBuffer[30], int i, int f, vector<int> meter, vector<int> buttons, action * c, SDL_Rect &p, int &cFlag, int &hFlag, bool aerial)
{
	if(aerial) return airHead->actionHook(inputBuffer, 0, -1, meter, buttons, c, p, cFlag, hFlag);
	else return avatar::hook(inputBuffer, 0, -1, meter, buttons, c, p, cFlag, hFlag, aerial);
}

action * avatar::moveSignal(int)
{
	return nullptr;
}

void avatar::neutralize(status &current, action *& cMove, vector<int> & meter)
{
	cMove = neutral;
	current.reversalFlag = false;
}

void character::neutralize(status &current, action *& cMove, vector<int> & meter)
{
	if(current.aerial) cMove = airNeutral;
	else cMove = neutral;
	current.reversalFlag = false;
}

void avatar::getName(string directory, string file)
{
	string buffer = "content/characters/" + directory + "/" + file + ".ch";
	ifstream read;

	read.open(buffer);
	if(read.fail()) getName("template", "template");

	read >> name;
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
		if(*moveList[i] == m->fileName && moveList[i]->typeKey == m->typeKey){
			if(m->null){
				moveList[i]->allowed.i = 0;
				//cout << "Disabling ";
			} else {
				char k = m->typeKey;
				moveList[i]->build(name, m->fileName);
				m->typeKey = k;
				processMove(moveList[i]);
				sortMove(moveList[i], input);
				//cout << "Replacing ";
			}
			replace = true;
			//cout << m->fileName + "\n";
			delete m;
			m = moveList[i];
			break;
		}
	}
	if(!replace){
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
	char buffer[101];
	bool commentFlag;
	ifstream read;
	read.open("content/characters/"+directory+"/"+file+".ch");

	if(read.fail()){ 
		cout << directory << "/" << file << " Character definition not found\n";
		return;
	}

	read.get(buffer, 50); read.ignore(100, '\n');

	while(!read.eof()){
		commentFlag = 0;
		read.get(buffer, 100, '\n'); read.ignore(100, '\n');
		dealWithMove(buffer);
	}
	read.close();
}

void avatar::sortMove(action * m, string key)
{
	tokenizer t(key, " \t=~>-&?@%$_!\n");
        t();
	while (t().size()){
		switch (t.current()[0]){
		case 'h':
			head->insert(m, t.current());
			break;
		default:
			break;
		}
	}
}

void avatar::loadAssets()
{
	for(action *i:moveList){ 
		if(i->payload) i->payload->loadAssets();
		i->loadMisc(name);
	}
}

void character::sortMove(action * m, string key)
{
	if(m->null) return;
	tokenizer t(key, " \t=>-&?@%$_!\n");
        t();
	while (t().size()){
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
	case '_':
		if(key[1] == 'j') m = new luftigeWerf(name, token);
		else m = new werf(name, token);
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

instance * avatar::spawn(action * source)
{
	return source->spawn();
}

void avatar::connect(status &current, vector<int>& meter)
{
	action * t = current.move->connect(meter, current.connect, current.frame);
	if(t != nullptr){
		current.bufferedMove = t;
	}
}

int character::checkBlocking(status &current, int input[])
{
	int st;
	if(current.hit < 0) current.hit = 0;
	if(current.connect < 0) current.connect = 0;
	bool success = false;
	int ret = -1;
	st = -(current.counter);
	if(st <= 0) st = 0;
	switch(input[0]){
	case 3:
	case 6:
	case 9:
		for(int i = 1; i < 5; i++){
			if(input[i] % 3 == 2){
				ret = 2;
				block(current, st, (input[0] > 3));
				break;
			}
		}
		break;
	case 7:
	case 4:
	case 1:
		block(current, st, (input[0] > 3));
		success = true;
		break;
	}
	if(success){
		ret = 0;
		for(int i = 1; i < 7; i++){
			if(input[i] % 3 != 1)
				ret = 1;
		}
	}
	return ret;
}

void character::block(status &current, int st, bool high)
{
	current.frame = 0;
	current.hit = 0;
	current.connect = 0;
	current.counter = -st;
	if(current.aerial){
		if(*airBlock > current) {
			current.move = airBlock;
		}
	} else {
		if(high){
			if(*standBlock > current) {
				current.move = standBlock;
			}
		} else {
			if(*crouchBlock > current) {
				current.move = crouchBlock;
			}
		}
	}
}

void avatar::pollRects(status& current, SDL_Rect& collision, vector<SDL_Rect>& hitreg, vector<SDL_Rect>& hitbox)
{
	current.move->pollRects(current.frame, current.connect, collision, hitreg, hitbox);
}

void avatar::pollStats(hStat & s, status &current)
{
	s.isProjectile = false;
	current.move->pollStats(s, current.frame, current.counter);
}

int character::takeHit(status &current, hStat & s, int blockType, int &hitType, vector<int>& meter)
{
	bool dead = false;
	int freeze = 0;
	if(s.pause < 0){
		freeze = s.stun/4 + 10;
		if(s.ghostHit) freeze = 0;
	} else freeze = s.pause;
	hitType = current.move->takeHit(s, blockType, current);
	if(hitType == 1) meter[0] -= s.damage;
	else if(hitType > -2) {
		meter[0] -= s.chip;
		if(hitType == -1 && meter[0] <= 0){ 
			meter[0] = 1;
		}
	}
	if(meter[0] <= 0){
		meter[0] = 0;
		dead = true;
	}
	if(dead == true){
		current.counter = -(s.stun+s.untech);
		die->execute(current.move, current, meter);
		current.move = die;
		current.aerial = true;
	} else if(hitType == 1) {
		if(s.launch) current.aerial = true;
		if(s.stun != 0){
			current.frame = 0;
			if(current.aerial){
				current.counter = -(s.stun+s.untech);
				current.move = untech;
				resetAirOptions(meter);
			} else if(current.move->crouch) {
				current.counter = -(s.stun + s.stun/5);
				current.move = crouchReel;
			} else {
				current.counter = -(s.stun);
				current.move = reel;
			}
		}
	} else if (hitType == -1) {
		if(!meter[4]){
			if(meter[1] + 6 < 300) meter[1] += 12;
			else meter[1] = 300;
		}
	}

	if (hitType == 1){ 
		if(!meter[4]){
			meter[1] += 2;
		}
	} else if (hitType > -2) {
		if(!meter[4]){
			if(meter[1] + 1 < 300) meter[1] += 3;
			else meter[1] = 300;
		}
	}
	return freeze;
}

vector<int> avatar::generateMeter()
{
	vector<int> meter(5);
	return meter;
}

void character::init(vector<int>& meter)
{
	meter[0] = 600;
	meter[1] = 0;
	resetAirOptions(meter);
	meter[4] = 0;
}

void character::resetAirOptions(vector<int>& meter)
{
	meter[2] = 1;
	meter[3] = 1;
}

int avatar::acceptTarget(action * c, int f)
{
	return 1;
}

void character::land(status& current, vector<int> & meter)
{
	if(current.move->allowed.b.block && current.counter < 0){
		current.move = standBlock;
	} else {
		current.move = current.move->land(current);
		if(!current.move) current.move = neutral;
	}
	resetAirOptions(meter);
}

void avatar::step(status &current, vector<int> & meter)
{
	int a = 0;
	if(current.move){
		if(current.move->hits > 0 && current.connect <= current.move->hits){
			a += current.move->stats[current.connect-1].connect;
			if(current.counter > 0)
				a += current.move->CHStats[current.connect-1].connect;
		}
	}
	if(current.freeze <= 0){
		current.move->step(meter, current);
		tick(meter);
		if(a < 0) current.connect += a;
		if(current.counter > 0) current.counter = 0;
	} else {
		current.freeze--;
		if(!current.freeze && a > 0){
			current.connect += a;
			current.hit += a;
		}
	}
	if(meter[4] > 0) meter[4]--;
}
