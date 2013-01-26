/*Header file for "Player" class in project Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012
 *Under MIT OSI license, see the file "COPYING" for details
 */
#include "../charlist.h"
#include "controller.h"
#ifndef PLAYER
#define PLAYER
class frame;
class instance{
public:
	instance();
	instance(avatar*);
	virtual ~instance();
	virtual avatar * pick() { return v; }

	SDL_Rect *hitbox, *hitreg, collision, *momentum;
	int hitComplexity, regComplexity, momentumComplexity;
	bool secondInstance;
	int posX, posY;
	int facing;
	int deltaX, deltaY; 
	int ID;
	int inputBuffer[30];
	int currentFrame;
	int freeze;
	action * cMove;
	action * bMove;
	action * sMove;
	int connectFlag, hitFlag;
	bool dead:1;
	int throwInvuln;
	virtual bool acceptTarget(instance*);
	virtual bool CHState() { return 0; }
	virtual void init();
	virtual void step();

	virtual void getMove(std::vector<int>, std::vector<bool>, SDL_Rect&, bool);
	virtual int takeHit(int, hStat&, SDL_Rect&);
	virtual void enforceGravity(int, int);
	virtual void invertVectors(int);
	virtual void addVector(SDL_Rect&);
	virtual void removeVector(int);
	virtual void pullVolition();
	virtual void setPosition(int, int);
	virtual void updateRects();
	virtual void draw(float);
	virtual void drawBoxen(float);
	virtual void combineDelta();
	virtual bool spriteCheck();
	virtual void connect(int, hStat&);
	virtual void pushInput(std::vector<bool>);
	virtual int passSignal(int);
	virtual int dragBG(int, int);
	void enforceAttractor(attractor*);
	int middle();
	void flip();

	bool aerial;
	int * meter;
	int particleType;
	int blockType;
	int counter;
	bool boxen:1;
	bool rCorner, lCorner;
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
	int padding[400];	//More magic. Do not touch
	virtual void characterSelect(int);
	virtual void drawHitParticle(int, int, float);

	virtual void land();
	virtual void enforceGravity(int, int);
	virtual void drawMeters(int, float);
	virtual void roundInit();
	virtual int takeHit(int, hStat&, SDL_Rect&);
//	virtual void connect(int, hStat&);
	virtual bool CHState();

	void setKey(int);
	bool setKey(int, SDL_Event);

	bool elasticX;
	bool elasticY;
	bool slide;
	bool stick;
	int hover;


	int particleLife;

	/*Helper functions for "resolve" tick*/
	virtual void checkBlocking();
	virtual void checkFacing(player*);
	virtual void checkCorners(int, int);
	virtual void enforceFloor(int);
	virtual void getThrown(action*, int, int);

	virtual void init();
	int wins;
private:
	character * v;
};

class demon : public player{
public:
	demon();
	demon(int);
	demon(int, frame*);
	frame * script;
	virtual void genEvent(bool *&, int *&, bool *&);
	virtual void roundInit();
	virtual void characterSelect(int);
	character * pick() { return v; }
private:
	character * v;
};
#endif
