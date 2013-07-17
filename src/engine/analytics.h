#include "player.h"

#ifndef ___analytics
#define ___analytics

using std::string;
using std::vector;

class chart{
public:
	chart(int);
	vector<vector<int> > wins;
	void recordWin(int, int);
	float matchup(int, int);
	void write();
	void init();
	~chart();
};

struct inputContainer {
	unsigned int dir:4;
	int A:2;
	int B:2;
	int C:2;
	int D:2;
	int E:2;
	bool Start:1;
//	bool Player:1; //What player the local client thinks it is
};

typedef union {
	inputContainer raw;
	int i;
} inputs;

struct frame {
	inputs n;
	vector<bool> axis;
	vector<int> buttons;
};

class script {
public:
	script() {}
	script(string);
	void init(int);
	void init(vector<int>);
	void push(int i, inputs);
	void push(inputs);
	vector<vector<inputs> > command;
	vector<int> selection;
	bool test();
	bool genEvent(int, int, inputs&);
	virtual void load(string);
	virtual void write(string);
	virtual void write();
	virtual ~script();
};

class HUD {
public:
	virtual void loadMisc();
	int freeze;
	vector<GLuint> glyph;
	vector<instance *> things;
	vector<int> combo;
	vector<int> damage;
	vector<bool> illegit;
	vector<int> punish;
	vector<int> counterHit;
	vector<int> blockFail;
	vector<SDL_Rect> hitloc;
};
#endif
