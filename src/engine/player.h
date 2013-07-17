/*Copyright Somnambulant Studios 2012-2013*/
#include "../charlist.h"
#include "controller.h"
#include <deque>
using std::deque;
#ifndef ___player
#define ___player

struct frame;
class instance{
public:
	instance();
	instance(avatar*);
	virtual ~instance();
	virtual avatar * pick() { return v; }

	SDL_Rect collision;
	vector<SDL_Rect> hitbox, hitreg, momentum;
	bool secondInstance;
	int ID;
	deque<int> inputBuffer;
	void checkFacing(instance*);
	virtual hStat pollStats();
	virtual void neutralize();
	virtual bool acceptTarget(instance*);
	virtual int CHState() { return 0; }
	virtual void init();
	virtual void step();

	virtual void getMove(vector<int>, bool&);
	virtual int takeHit(int, hStat&, SDL_Rect&);
	virtual void enforceGravity(int, int);
	virtual void invertVectors(int);
	virtual void pullVolition();
	virtual void setPosition(int, int);
	virtual void updateRects();
	virtual void draw();
	virtual instance* spawn();
	virtual void drawBoxen();
	virtual void combineDelta();
	virtual bool spriteCheck();
	virtual void connect(int, hStat&);
	virtual void pushInput(unsigned int);
	virtual int passSignal(int);
	virtual int dragBG(int, int);
	virtual void follow(instance*);
	virtual void loadAssets();
	void enforceAttractor(attractor*);
	int middle();
	void flip();
	void print();

	status current;
	status save;

	int particleType;
	int blockType;
	bool boxen:1;
	bool sprite:1;
protected:
	avatar * v;
};

class player : public instance, public controller{
public:
	player();
	player(int);
	~player();
	virtual character * pick() { return v; }

	int rounds;		//How many rounds has this player won this match?
//	int padding[400];	//More magic. Do not touch
	virtual void characterSelect(int);
	virtual void drawHitParticle();

	virtual void readEvent(SDL_Event &, frame &);
	virtual void land();
	virtual void readScripts();
	virtual void enforceGravity(int, int);
	virtual void drawMeters(int);
	virtual void roundInit();
	virtual void macroCheck(SDL_Event&);
	virtual int takeHit(int, hStat&, SDL_Rect&);
//	virtual void connect(int, hStat&);
	virtual int CHState();
	virtual bool reversalPossible();

	void setKey(int);
	bool setKey(int, SDL_Event);

	bool elasticX;
	bool elasticY;
	bool slide;
	bool stick;
	int hover;

	string name;

	int particleLife;

	/*Helper functions for "resolve" tick*/
	virtual void checkBlocking();
	virtual void checkCorners(int, int);
	virtual void enforceFloor(int);
	virtual void getThrown(action*, int, int);

	virtual void init();
	int wins;
	bool search:1;
	script * record;
private:
	character * v;
};
#endif
