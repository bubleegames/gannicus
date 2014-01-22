/*Copyright Somnambulant Studios 2012-2013*/
#include "character.h"
#include "../charlist.h"
#include "controller.h"
#include <deque>
using std::deque;
#ifndef ___player
#define ___player
class force;
struct frame;
class instance{
public:
	instance();
	instance(avatar*);
	virtual ~instance();
	virtual avatar * pick() { return v; }
	vector<SDL_Rect> momentum;
	vector<SDL_Rect> hitLocation;
	int selectedPalette;
	int ID;
	deque<int> inputBuffer;
	void checkFacing();
	void checkReversal();
	void checkFacing(instance*);
	virtual bool isDead();
	virtual hStat pollStats();
	virtual void neutralize();
	virtual bool validate(int, int);
	virtual bool acceptTarget(instance*);
	virtual void outOfBounds();
	virtual void manageOffspring();
	virtual int CHState() const { return 0; }
	virtual void init();
	virtual void step();

	virtual void getMove(vector<int>);
	virtual int takeHit(int, hStat&);
	virtual void invertVectors(int);
	virtual void pullVolition();
	virtual void setPosition(int, int);
	virtual void updateRects();
	virtual void draw(GLint);
	virtual instance* spawn();
	virtual bool stuck();
	virtual void drawBoxen();
	virtual void combineDelta();
	virtual int comboState();
	virtual bool spriteCheck();
	virtual void connect(int, hStat&);
	virtual void checkBlocking() {}
	virtual void pushInput(unsigned int);
	virtual void pushInput(deque<int>);
	virtual void passSignal(int);
	virtual int dragBG(int, int);
	virtual void follow();
	virtual void follow(instance*);
	virtual void loadAssets();
	virtual bool checkHit(instance*);
	virtual void land();
	virtual void cleanup();
	virtual void encounterWall(bool, int);
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
	virtual bool isDead();
	virtual character * pick() { return v; }

	int rounds;		//How many rounds has this player won this match?
//	int padding[400];	//More magic. Do not touch
	virtual int comboState();
	virtual void characterSelect(int);
	virtual void enemySelect(int);
	virtual bool stuck();
	virtual bool validate(int, int);
	virtual void drawHitParticle();

	virtual void readEvent(SDL_Event &, frame &);
	virtual void readScripts();
	virtual void roundInit();
	virtual void macroCheck(SDL_Event&);
	virtual void outOfBounds() {}
	virtual void land();
	virtual int takeHit(int, hStat&);
	virtual int CHState() const;
	virtual bool reversalPossible() const;

	void setKey(int);
	bool setKey(int, SDL_Event);

	string name;

	int particleLife;

	/*Helper functions for "resolve" tick*/
	virtual void checkBlocking();
	virtual void getThrown(action*, int, int);

	virtual void init();
	int wins;
	bool search:1;
	script * record;
private:
	character * v;
};
#endif
