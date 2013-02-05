/*Header file for "action" class in project Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012.
 *Licensed under MIT OSI, see "COPYING" for details
 */

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <vector>
#include "auxil.h"
#include "masks.h"
#ifndef ACTION
#define ACTION
class avatar;
class instance;
struct hStat{
	hStat() : damage(0), chip(0), stun(0), pause(-1), push(0), lift(0), untech(0), blowback(0), hover(0), launch(0), ghostHit(0), wallBounce(0), floorBounce(0), slide(0), stick(0), hitsProjectile(0), turnsProjectile(0), killsProjectile(0), noConnect(0) {}
	int damage;	/*How much damage the hit does*/
	int chip;	/*How much damage the hit does if blocked*/
	int stun;	/*How many frames of stun the hit causes*/
	int pause;
	int push;	/*How many pixels the hit pushes the opponent back*/
	int lift;	/*How many pixels the hit lifts an aerial opponent.*/
	int untech;	/*How many more frames of stun are added for an aerial hit*/
	int blowback;	/*How many pixels per frame are added to push in the air*/
	int hover;
	bool launch:1;	/*Does this hit put the opponent in the air*/
	bool ghostHit:1;
	bool wallBounce:1;
	bool floorBounce:1;
	bool slide:1;
	bool stick:1;
	bool hitsProjectile:1;
	bool turnsProjectile:1;
	bool killsProjectile:1;
	bool noConnect:1;
	blockField blockMask;
	cancelField hitState;
};

class action{
public:
	action();
	action(const char*);
	virtual ~action();
	bool spriteCheck(int);
	virtual void build(const char *);
	virtual void loadMisc(const char *);

	//Okay so, hopefully the idea here is that we can init()
	//the action we're cancelling out of in the usual case, and, well
	//Do other stuff sometimes.
	virtual void execute(action *, int *&, int&, int&, int&);
	virtual void init(int) {}
	virtual void playSound(int);
	virtual bool activate(std::vector<int>, std::vector<bool>, int, int, int, int[], SDL_Rect&); //Check to see if the action is possible right now.
	virtual void generate(const char*, const char*);
	virtual bool check(SDL_Rect&, int[]); //Check to see if the action is possible right now.
	virtual action * blockSuccess();
	virtual int arbitraryPoll(int q, int f);

	//Return the relevant information needed for interface::resolve(), then step to the next frame.
	virtual void pollRects(SDL_Rect&, SDL_Rect*&, int&, SDL_Rect*&, int&, int, int);
	virtual void pollDelta(SDL_Rect *&, int&, int);
	virtual int displace(int, int&, int);
	Mix_Chunk *soundClip;
	virtual void pollStats(hStat&, int, bool);
	virtual bool cancel(action*, int&, int&); //Cancel allowed activate. Essentially: is action Lvalue allowed given the current state of action Rvalue?
	virtual void step(int *&, int&, int&, int&);
	virtual action * land(int &f, int &h, int &c) { return this; }
	virtual action * connect(int *&, int&, int);
	virtual instance * spawn();
	virtual int takeHit(hStat&, int, int&, int&, int&); 

	virtual void feed(action *, int, int);
	virtual char* request(int, int);

	bool CHState(int);
	virtual void draw(int);
	virtual void drawBoxen(int, int, int);

	hStat *stats, *CHStats;
	int stop;
	int throwinvuln;
	bool crouch:1;
	bool hittable:1;
	bool hidesMeter:1;
	int armorStart; int armorLength;
	int armorHits;
	int armorCounter;
	int guardStart; int guardLength;
	int freezeFrame; int freezeLength;

	//Properties of a hit. These will only exist for actions that hit.
	
	//Low, High, Air Block. Basically a 3-digit binary number expressed as an integer from 0-7.
	//Digits from low to high: Blockable low, Blockable high, Blockable in the air, use a bitmask

	blockField blockState;

	//Cancel states, as defined in masks.h. Eventually this will probably be an array.

	cancelField *state, allowed;
	int xRequisite, yRequisite;

	int frames;	//Number of frames.
	int hits;
	int * totalStartup;
	int * active;
	bool fch:1;
	bool dies:1;

	//SDL_Surface *sprite, *hit, *hitreg, *collision;
	char * name;
	int cost;
	int * gain;

	//Tolerance refers to the individual size of the input buffer allowed for this action.
	//Default is 30 (The entire input buffer)
	int tolerance;

	//Activation refers to the most recent accepted last command. So an activation of 0
	//Implies that the most recent input has to have been *this frame* for the action to be
	//Accepted. Default is 30 (the entire input buffer)
	int activation;

	int minHold, maxHold;

	action * next;
	action ** onConnect;
	action * onHold;
	action * attempt;
	action * riposte;

	action * basis;
	int connectFlag, currentFrame, hitFlag;
	int holdFrame;
	int holdCheck;

	attractor * distortion;
	int distortSpawn;
	int attemptStart, attemptEnd;
	int displaceX, displaceY, displaceFrame;
	bool window(int);
	int calcCurrentHit(int);

	char * tempNext;
	char ** tempOnConnect;
	char * tempAttempt;
	char * tempRiposte;
	char * tempOnHold;

	SDL_Rect * collision;   //This will be an array of rects that are the collision boxes for the action per frame
	SDL_Rect ** hitbox;     //Same but for hitboxes
	SDL_Rect ** hitreg;     //Same but for hitreg boxes
	SDL_Rect ** delta;       //Same but for position on the screen.
	int * hitComplexity;
	int * regComplexity;
	int * deltaComplexity;

	std::vector<int> width, height;
	std::vector<GLuint> sprite;

	bool isProjectile:1;
	bool modifier:1;
	virtual bool setParameter(char*);
	virtual void parseProperties(char*, bool);
	virtual void zero();

	//Projectile stuff;
	avatar * payload;
	char * tempPayload;
	int spawnFrame;
	int spawnPosX;
	int spawnPosY;
	int lifespan;
	int allegiance;
	bool spawnTrackX:1;
	bool spawnTrackY:1;
	bool spawnTrackFloor:1;
};

class hitstun : virtual public action {
public:
	hitstun() {}
	void init(int);
	int counter;
	virtual void step(int *&, int&, int&, int&);
	virtual action * blockSuccess(int);
	virtual int takeHit(hStat&, int, int&, int&, int&); 
	virtual int arbitraryPoll(int, int);
	hitstun(char *, int);
	hitstun(const char *);
};

class special : virtual public action {
public:
	special() {}
	special(const char*);
	virtual bool activate(std::vector<int>, std::vector<bool>, int, int, int, int[], SDL_Rect&); //Check to see if the action is possible right now.
};

class negNormal : virtual public action {
public:
	negNormal() {}
	negNormal(const char *);
	virtual void zero();
	virtual bool activate(std::vector<int>, std::vector<bool>, int, int, int, int[], SDL_Rect&); //Check to see if the action is possible right now.
};

class utility : virtual public action {
public:
	utility() {}
	utility(const char *);
	virtual bool activate(std::vector<int>, std::vector<bool>, int, int, int, int[], SDL_Rect&); //Check to see if the action is possible right now.
};

class looping : virtual public utility {
public:
	looping() {}
	looping(const char*);
	virtual void step(int *&, int&, int&, int&);
};

class airMove : virtual public action {
public:
	airMove() {}
	airMove(const char*);
	virtual void build (const char *);
	virtual action * land(int&, int&, int&);
	char * tempLanding;
	virtual bool setParameter(char*);
	virtual void feed(action *, int, int);
	virtual char* request(int, int);
	action * landing;
	virtual void zero() { tempLanding = NULL; landing = NULL; action::zero(); }
};

class untechState : public airMove, public hitstun {
public:
	untechState() {}
	untechState(const char*);
};


class airSpecial : public airMove, public special {
public:
	airSpecial() {}
	airSpecial(const char* n) {build(n); }
};

class airNegNormal : public airMove, public negNormal {
public:
	airNegNormal() {}
	airNegNormal(const char* n) {build(n); }
	void zero() { airMove::zero(); negNormal::zero(); }
};

class airUtility : public airMove, public utility {
public:
	airUtility() {}
	airUtility(const char*);
	virtual bool check(SDL_Rect&, int[]); //Check to see if the action is possible right now.
	virtual void execute(action *, int *&, int&, int&, int&);
};

class airLooping : public airMove, public looping {
public:
	airLooping() {}
	airLooping(const char*);
};

class mash : virtual public action {
public:
	mash() {}
	mash(const char* n) {build(n); }
	virtual bool setParameter(char *n);
	virtual void zero();
	virtual bool activate(std::vector<int>, std::vector<bool>, int, int, int, int[], SDL_Rect&); //Check to see if the action is possible right now.
	int buttons;
};

class werf : virtual public action {
public:
	werf() {}
	werf(const char* n) {build(n); }
	virtual bool setParameter(char *n);
	virtual bool check(SDL_Rect&, int[]); //Check to see if the action is possible right now.
	virtual int arbitraryPoll(int, int);
	int startPosX;
	int startPosY;
};

class luftigeWerf : public airMove, public werf {
public:
	luftigeWerf() {}
	luftigeWerf(const char* n) {build(n); }
	virtual bool setParameter(char *n);
	void build(const char *n) {werf::build(n);}
	virtual bool check(SDL_Rect&, int[]); //Check to see if the action is possible right now.
};

class releaseCheck : virtual public action {
public:
	releaseCheck() {}
	releaseCheck(const char* n) {build(n); }
	virtual bool activate(std::vector<int>, std::vector<bool>, int, int, int, int[], SDL_Rect&); //Check to see if the action is possible right now.
};
#endif
