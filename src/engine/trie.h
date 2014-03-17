/*Copyright Somnambulant Studios 2012-2013*/
#include "action.h"
#include <deque>
using std::deque;
#ifndef ___move_trie
#define ___move_trie
class actionHandle{
public:
	actionHandle() : target(nullptr), pattern(0) {}
	actionHandle(action* t, int p) { target = t; pattern = p; }
	unsigned int patternMatch(int[], bool[]);
	action * target;
	int pattern;
};

class actionTrie{
public:
	actionTrie();
	actionTrie(action*);
	actionTrie(action*, int);
	~actionTrie();
	action * actionHook(status&, deque<int>, int, int, vector<int>);
	actionTrie * child[10];
	deque<actionHandle> fish;
	actionTrie * insert(int);
	actionTrie * insert(int, action*);
	void insert(action *, int);
	void insert(action *, string);
private:
	vector<int> pattern;
};
#endif
