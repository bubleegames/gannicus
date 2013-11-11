#include "../engine/interface.h"
red::red()
{
	airHead = new actionTrie;
	head = new actionTrie;
	build("Red", "Red");
	backup = new instance;
}

vector<HUDMeter<int>> red::generateMeter()
{
	vector<HUDMeter<int>> meter = character::generateMeter();
	meter.push_back(HUDMeter<int>(540));
	meter.push_back(HUDMeter<int>(540));
	meter[5].w = 0;
	meter[6].w = 0;
	return meter;
}

void red::tick(status &current)
{
	character::tick(current);
	if(current.meter[5].value < 540) current.meter[5].value++;
	if(current.meter[5].value < 0) current.meter[5].value = 0;
}

void red::step(status& current)
{
	if(current.meter[6].value > 0) current.meter[6].value--;
	temporalBuffer.push_back(current);
	if(temporalBuffer.size() > 120) temporalBuffer.erase(temporalBuffer.begin());
	character::step(current);
}

void red::init(status& current)
{
	character::init(current);
	current.meter[5].value = 540;
	current.meter[6].value = 0;
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

bool redCancel::check(const status &current)
{
	if(current.meter[6].value > 0) return 0;
	return special::check(current);
}

action * redCancel::execute(status &current)
{
	current.meter[2].value = 1;
	current.meter[3].value = 1;
	current.meter[6].value = 16;
	return action::execute(current);
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
