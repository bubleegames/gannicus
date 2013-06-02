#include "../engine/interface.h"
red::red()
{
	airHead = new actionTrie;
	head = new actionTrie;
	build("Red", "Red");
	backup = new instance;
}

vector<int> red::generateMeter()
{
	vector<int> meter (7);
	return meter;
}

void red::tick(status &current)
{
	character::tick(current);
	if(current.meter[5] < 540) current.meter[5]++;
	if(current.meter[5] < 0) current.meter[5] = 0;
}

void red::step(status& current)
{
	if(current.meter[6] > 0) current.meter[6]--;
	temporalBuffer.push_back(current);
	if(temporalBuffer.size() > 120) temporalBuffer.erase(temporalBuffer.begin());
	character::step(current);
}

void red::init(status& current)
{
	character::init(current);
	current.meter[5] = 540;
	current.meter[6] = 0;
}

action * red::createMove(string key)
{
	tokenizer t(key, " \t-@?_%$!\n");
        t();
	action * m;
	switch(key[0]){
	case '$':
		if(key[1] == '!') m = new redSuper(name, t.current());
		else m = new redCancel(name, t.current());
		break;
	default:
		m = character::createMove(key);
		break;
	}
	if(m->typeKey == '0') m->typeKey = key[0];
	return m;
}

redCancel::redCancel(string dir, string file) 
{
	build(dir, file); 
}

bool redCancel::check(status &current)
{
	if(current.meter[6] > 0) return 0;
	return special::check(current);
}

void redCancel::execute(status &current)
{
	current.meter[2] = 1;
	current.meter[3] = 1;
	current.meter[6] = 16;
	action::execute(current);
}

int redCancel::arbitraryPoll(int q, int f)
{
	if(q == 33) return 1;
	else return action::arbitraryPoll(q, f);
}
int redSuper::arbitraryPoll(int q, int f)
{
	if(q == 31) return 11;
	else if(q == 32) return 1;
	else return action::arbitraryPoll(q, f);
}

redCancel::~redCancel() {}
redSuper::~redSuper() {}
