/*Header file for character class in project Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012
 *Under MIT OSI license, see "COPYING" for details
 */
#include <SDL/SDL.h>
#include "trie.h"
#ifndef CHARACTER
#define CHARACTER
class instance;
class avatar {
public:
	avatar() {}
	virtual int passSignal(int) { return 0; }
	virtual void setParam(int, int) {}
	virtual int * generateMeter();
	virtual void init(int*&) = 0;
	virtual action * moveSignal(int);
	bool spriteCheck(action *&, int);
	virtual void draw(action *&, int);//Pass sprite information up.
	virtual action * createMove(char*);
	virtual void build(const char*, const char*);
	virtual void processMove(action * m);
	virtual void sortMove(action *, char*);
	virtual void prepHooks(status&, action *&, int[], std::vector<int>, std::vector<bool>, SDL_Rect &, bool, int*&);	//Take input from the game and propagate it to the appropriate actionTrie.
	//BRB prepping my hooks
	virtual action * hook(int[], int, int, int*, std::vector<int>, std::vector<bool>, action *, SDL_Rect&, int&, int&, bool);
	virtual bool death(action *&, int, int) { return 0; }
	virtual int takeHit(status&, hStat&, int, int&, int*&) { return 0; }
	virtual void getName(const char*, const char*);
	virtual void connect(status&, int*&);
	virtual void step(status&, int*&);
	virtual int acceptTarget(action*, int);
	virtual instance * spawn(action*);
	virtual void tick(int *&) {}
	virtual void neutralize(status&, action*&, int*&);
	virtual bool turn(int&) { return 0; }
	char * name; //The name of the directory from which the character spawns. This is important for loading into memory
	actionTrie * head;	//Trie for ground actions
	int lifespan;

	action * die;
	action * neutral;
	bool headless;
	std::vector<action*> movesByName;
	action * searchByName(char*);
};

class character : virtual public avatar{
public:
	bool dFlag:1;
	character(const char*);
	actionTrie * airHead;	//Trie for air actions
	character();		//Load the entire character into memory. This should happen once per player per match.
	virtual ~character();	//Free stuff

	virtual void build(const char*, const char*);//This will *eventually* be the function that parses the character constructor file.

	virtual int comboState(action *);
	virtual void neutralize(status&, action*&, int*&);
	virtual void drawMeters(int, int, int*);
	virtual void init(int *&);
	virtual int checkBlocking(action *&, int[], int&, int&, bool);
	virtual void resetAirOptions(int *&);
	virtual void land(action *&, int &, int &, int &, int *&);
	virtual void sortMove(action *, char*);
	virtual int takeHit(status&, hStat&, int, int&, int*&);
	virtual action * hook(int[], int, int, int*, std::vector<int>, std::vector<bool>, action *, SDL_Rect&, int&, int&, bool);

	looping * dead;
	action * airNeutral;
	action * crouch;
	hitstun * reel;
	untechState * untech;
	airLooping * fall;
	utility * down;
	hitstun * crouchReel;
	hitstun * crouchBlock;
	hitstun * standBlock;
	hitstun * airBlock;
	utility * throwBreak;
};

class projectile : virtual public avatar {
public:
	projectile(const char* directory, const char* file);
	projectile() {}
	virtual void build(const char*, const char*);
	virtual int acceptTarget(action*, int);
	virtual int takeHit(status&, hStat&, int, int&, int*&);
	virtual bool turn(int&);

	virtual void processMove(action * m);
	virtual bool death(action *&, int, int);
	virtual void init(int *&);
};
#endif
