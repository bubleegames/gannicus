#include <SDL/SDL.h>
#include <string>
#include <vector>
#include <deque>

#ifndef ___controller
#define ___controller

using std::deque;
using std::string;
using std::vector;

struct frame;
class script;


struct keySetting{
	keySetting();
	SDL_Event trigger;
	int effect;
};

class inputHandler {
public:
	vector<keySetting*> input;
	vector<string> inputName;//This is really just for housekeeping.
	virtual int tap(const SDL_Event&);
};

class controller : public inputHandler {
public:
	virtual void init();
	deque<int> inputBuffer;
	virtual void readEvent(SDL_Event &, frame&);
	virtual void setKey(int);
	void writeConfig(int);
	bool readConfig(int);
	virtual void pushInput(unsigned int);
	virtual bool setKey(int, SDL_Event);
	virtual bool same(SDL_Event);
	virtual int tap(const SDL_Event&);
	virtual void swapKey(int, SDL_Event);
	vector<script*> macro;
	vector<int> pattern;
	script * patternMatch(int);
	script * currentMacro;
	int iterator;
};
#endif
