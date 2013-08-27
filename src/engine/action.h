/*Copyright Somnambulant Studios 2012-2013*/
#include "auxil.h"
#include "masks.h"
#include "sdl-compat.h"
#include "tokenizer.h"
#include <iostream>
#include <fstream>

#ifndef ___action
#define ___action

using std::string;
using std::vector;
using std::ifstream;
class actionTrie;
struct animation{
	int frames;	//Number of frames.
	virtual void draw(int) = 0;
};

class avatar;
class instance;
struct hStat{
	hStat() : damage(0), chip(0), stun(0), initialLaunch(0), pause(-1), push(0), lift(0), untech(0), blowback(0), hover(0), launch(0), ghostHit(0), wallBounce(0), floorBounce(0), slide(0), stick(0), hitsProjectile(0), turnsProjectile(0), killsProjectile(0), isProjectile(0), autoCorrects(0), connect(0), prorate(1.0) {}
	hStat(const hStat&);
	int damage;	/*How much damage the hit does*/
	int chip;	/*How much damage the hit does if blocked*/
	int stun;	/*How many frames of stun the hit causes*/
	int initialLaunch;	/*Additional untech for launching them*/
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
	bool isProjectile:1;
	bool autoCorrects:1;
	int connect;
	float prorate;
	blockField blockMask;
	cancelField hitState;
};

class action : public animation{
public:
	action();
	action(string, string);
	char typeKey;
	virtual ~action();
	int requiredMode;
	bool spriteCheck(int);
	virtual void build(string, string);
	virtual bool parseRect(string);
	virtual void loadMisc(string);

	//Okay so, hopefully the idea here is that we can init()
	//the action we're cancelling out of in the usual case, and, well
	//Do other stuff sometimes.
	virtual void execute(status&);
	virtual void playSound(int);
	virtual bool patternMatch(vector<int>, int, int, int); //Check to see if the action is possible right now.
	virtual bool activate(status &, vector<int>, int, int, int); //Check to see if the action is possible right now.
	virtual void generate(string, string);
	virtual bool check(const status&); //Check to see if the action is possible right now.

	virtual action * blockSuccess(int, bool);
	virtual int arbitraryPoll(int q, int f);

	//Return the relevant information needed for interface::resolve(), then step to the next frame.
	virtual void pollRects(int, int, SDL_Rect&, vector<SDL_Rect>&, vector<SDL_Rect>&);
	virtual vector<SDL_Rect> pollDelta(int);
	virtual int displace(int, int&, int);
	virtual hStat pollStats(int, bool);
	virtual bool cancel(action*, int, int); //Cancel allowed patternMatch. Essentially: is action Lvalue allowed given the current state of action Rvalue?
	virtual void step(status&); //Step forward one frame. This only happens if we're not in freeze state
	virtual action * land(status&) { return this; }
	virtual action * connect(vector<int>&, int&, int);
	virtual instance * spawn();
	virtual int takeHit(hStat&, int, status&);
	virtual bool canGuard(int);

	virtual void feed(action *, int, int);
	virtual string request(int, int);

	virtual void draw(int);
	virtual void drawBoxen(int);
	virtual bool CHState(int);

	virtual bool window(int);
	int calcCurrentHit(int);

	vector<hStat> stats, CHStats;
	Mix_Chunk *soundClip;
	int stop;
	int throwinvuln;
	bool null:1;
	bool crouch:1;
	bool hittable:1;
	bool linkable:1;
	bool hidesMeter:1;
	bool track:1;
	bool countersProjectile:1;
	int flip;
	int armorStart, armorLength;
	int armorHits, armorCounter;
	int guardStart, guardLength, guardType, 
		stunMin, stunMax;
	int freezeFrame, freezeLength;
	int followStart, followEnd;
	int followXRate, followYRate;
	int offX, offY;

	//Properties of a hit. These will only exist for actions that hit.
	
	//Low, High, Air Block. Basically a 3-digit binary number expressed as an integer from 0-7.
	//Digits from low to high: Blockable low, Blockable high, Blockable in the air, use a bitmask

	blockField blockState;

	//Cancel states, as defined in masks.h. Eventually this will probably be an array.

	vector<cancelField> state; 
	cancelField allowed;
	int xRequisite, yRequisite;

	int hits;
	vector<int> totalStartup;
	vector<int> active;
	bool fch:1;
	bool dies:1;

	//SDL_Surface *sprite, *hit, *hitreg, *collision;
	string name;
	string fileName;
	int cost;
	vector<int> gain;

	//Tolerance refers to the individual size of the input buffer allowed for this action.
	//Default is 30 (The entire input buffer)
	int tolerance;

	//Activation refers to the most recent accepted last command. So an activation of 0
	//Implies that the most recent input has to have been *this frame* for the action to be
	//Accepted. Default is 30 (the entire input buffer)
	int activation;

	int minHold, maxHold;

	action * next;
	vector<action *> onConnect;
	actionTrie * followup;
	action * onHold;
	action * attempt;
	action * riposte;

	status basis;
	int holdFrame;
	int holdCheck;

	attractor * distortion;
	int distortSpawn;
	span<int> followupSpan;
	span<int> attemptSpan;
	int displaceX, displaceY, displaceFrame;

	string tempNext;
	vector<string> tempFollowup;
	vector<string> tempOnConnect;
	string tempAttempt;
	string tempRiposte;
	string tempOnHold;

	vector<SDL_Rect> collision;   //This will be an array of rects that are the collision boxes for the action per frame
	vector<vector<SDL_Rect> > hitreg, hitbox, delta;       //Same but for position on the screen.

	vector<int> width, height;
	vector<GLuint> sprite;

	bool modifier:1;
	virtual bool setParameter(string);
	virtual void parseProperties(string, bool);
	virtual void zero();

	//Projectile stuff;
	avatar * payload;
	string tempPayload;
	string tempParticle;
	int particleSpawn;
	int particleX, particleY;
	int spawnFrame;
	int spawnPosX, spawnPosY;
	int lifespan;
	int allegiance;
	bool spawnTrackX:1;
	bool spawnTrackY:1;
	bool spawnTrackFloor:1;

	vector <pixelMap> shadedSprite;

	bool operator!=(const string&);
	bool operator==(const string&);
	bool operator>(const status&);
};

class hitstun : virtual public action {
public:
	hitstun() {}
	virtual void step(status&);
	virtual int takeHit(hStat&, int, status&); 
	virtual bool canGuard(int);
	hitstun(string, string);
};

class special : virtual public action {
public:
	special() {}
	special(string, string);
	virtual bool check(const status&); //Check to see if the action is possible right now.
	virtual bool patternMatch(vector<int>, int, int, int); //Check to see if the action is possible right now.
};

class negNormal : virtual public action {
public:
	negNormal() {}
	negNormal(string, string);
	virtual void zero();
	virtual bool patternMatch(vector<int>, int, int, int); //Check to see if the action is possible right now.
};

class utility : virtual public action {
public:
	utility() {}
	utility(string, string);
	virtual bool patternMatch(vector<int>, int, int, int); //Check to see if the action is possible right now.
};

class looping : virtual public utility {
public:
	looping() {}
	looping(string, string);
	virtual void step(status&);
};

class airMove : virtual public action {
public:
	airMove() {}
	airMove(string, string);
	virtual action * land(status&);
	string tempLanding;
	virtual void zero();
	virtual bool setParameter(string);
	virtual void feed(action *, int, int);
	virtual string request(int, int);
	action * landing;
};

class untechState : public airMove, public hitstun {
public:
	untechState() {}
	untechState(string, string);
};


class airSpecial : public airMove, public special {
public:
	airSpecial() {}
	airSpecial(string d, string f) { build(d,f); }
};

class airNegNormal : public airMove, public negNormal {
public:
	airNegNormal() {}
	airNegNormal(string d, string f) { build(d,f); }
	virtual void zero() { airMove::zero(); }
};

class airUtility : public airMove, public utility {
public:
	airUtility() {}
	airUtility(string, string);
	virtual bool check(const status&); //Check to see if the action is possible right now.
	virtual void execute(status&);
};

class airLooping : public airMove, public looping {
public:
	airLooping() {}
	airLooping(string, string);
};

class mash : virtual public action {
public:
	mash() {}
	mash(string d, string f) { build(d,f); }
	virtual bool setParameter(string);
	virtual void zero();
	virtual bool patternMatch(vector<int>, int, int, int); //Check to see if the action is possible right now.
	int buttons;
};

class werf : virtual public action {
public:
	werf() {}
	werf(string d, string f) { build(d,f); }
	virtual bool setParameter(string);
	virtual bool check(const status&); //Check to see if the action is possible right now.
	virtual int arbitraryPoll(int, int);
	int startPosX;
	int startPosY;
};

class luftigeWerf : public airMove, public werf {
public:
	luftigeWerf() {}
	luftigeWerf(string d, string f) { build(d,f); }
	virtual bool setParameter(string);
	virtual bool check(const status&); //Check to see if the action is possible right now.
};

class releaseCheck : virtual public action {
public:
	releaseCheck() {}
	releaseCheck(string d, string f) { build(d,f); }
	virtual bool patternMatch(vector<int>, int, int, int); //Check to see if the action is possible right now.
};

class airReleaseCheck : public airMove, public releaseCheck {
public:
	airReleaseCheck() {}
	airReleaseCheck(string d, string f) { build(d,f); }
};
#endif
