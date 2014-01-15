#include "../engine/interface.h"
red::red()
{
	airHead = new actionTrie;
	head = new actionTrie;
	build("Red", "Red");
	backup = new instance;
}

void red::step(status& current)
{
	temporalBuffer.push_back(current);
	if(temporalBuffer.size() > 120) temporalBuffer.erase(temporalBuffer.begin());
	character::step(current);
}

action * red::createMove(string key)
{
	tokenizer t(key, " \t-@?_%$!\n");
        t();
	action * m;
	switch(key[0]){
	case '$':
		m = new redSuper(name, t.current());
		break;
	default:
		m = character::createMove(key);
		break;
	}
	if(m->typeKey == '0') m->typeKey = key[0];
	return m;
}

int redSuper::arbitraryPoll(int q, int f)
{
	if(q == 31) return 11;
	else if(q == 32) return 1;
	else return action::arbitraryPoll(q, f);
}

redSuper::~redSuper() {}
