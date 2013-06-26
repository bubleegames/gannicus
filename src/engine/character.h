/*Copyright Somnambulant Studios 2012-2013*/
#include "trie.h"
#include <deque>

#ifndef ___character
#define ___character

using std::deque;
using std::string;
using std::vector;

class instance;
class avatar {
public:
	avatar() {}
	virtual void setParam(int, int) {}
	virtual vector<int> generateMeter();
	virtual void init(status&) = 0;
	virtual action * moveSignal(int);
	bool spriteCheck(action *&, int);
	virtual void draw(action *&, int);
	virtual action * createMove(string);
	virtual action * dealWithMove(string);
	virtual action * mandateMove(string);
	virtual void build(string, string);
	virtual void processMove(action * m);
	virtual void sortMove(action *, string);
	virtual void prepHooks(status&, deque<int>, vector<int>, bool);
	virtual void checkReversal(status&, bool);
	virtual void getReversal(status &, deque<int>, vector<int>);
	virtual void executeBuffer(status &, bool);
	virtual action * hook(status&, deque<int>, vector<int>);
	virtual bool death(status&) { return 0; }
	virtual int takeHit(status&, hStat&, int, int&) { return 0; }
	virtual void getName(string, string);
	virtual void loadAssets();
	virtual void connect(status&);
	virtual hStat pollStats(status&);
	virtual void pollRects(status&, SDL_Rect&, vector<SDL_Rect>&, vector<SDL_Rect>&);
	virtual void step(status&);		//Anything that happens every frame
	virtual void tick(status&) {}	//Anything that happens every frame as long as we're not in freeze state
	virtual int acceptTarget(action*, int);
	virtual instance * spawn(status&);
	virtual action * neutralize(status&);
	virtual bool turn(int&) { return 0; }
	string name;
	actionTrie * head;
	int lifespan;

	action * die;
	action * neutral;
	bool headless;
	vector<action*> moveList;
	vector<string> moveNames;
};

class character : virtual public avatar{
public:
	bool dFlag:1;
	character(string);
	actionTrie * airHead;
	character();
	virtual ~character();

	virtual void build(string, string);
	virtual int comboState(action *);
	virtual action * neutralize(status&);
	virtual void drawMeters(int, int, status&);
	virtual void init(status&);
	virtual int checkBlocking(status&, deque<int>);
	virtual void block(status&, int, bool);
	virtual void resetAirOptions(vector<int>&);
	virtual void land(status&);
	virtual void sortMove(action *, string);
	virtual int takeHit(status&, hStat&, int, int&);
	virtual action * hook(status&, deque<int>, vector<int>);

	action * dead;
	action * airNeutral;
	action * crouch;
	action * reel;
	action * untech;
	action * fall;
	action * down;
	action * crouchReel;
	action * crouchBlock;
	action * standBlock;
	action * airBlock;
	action * throwBreak;
};

class projectile : virtual public avatar {
public:
	projectile(string, string);
	projectile() {}
	virtual void build(string, string);
	virtual int acceptTarget(action*, int);
	virtual hStat pollStats(status&);
	virtual int takeHit(status&, hStat&, int, int&);
	virtual bool turn(int&);

	virtual void processMove(action * m);
	virtual bool death(status&);
	virtual void init(status&);
};
#endif
