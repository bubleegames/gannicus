/*Copyright Somnambulant Studios 2012-2013*/
#include "action.h"
#include <deque>
using std::deque;
#ifndef ___move_trie
#define ___move_trie

class actionTrie{
public:
	actionTrie();
	actionTrie(action*);
	actionTrie(action*, int);
	~actionTrie();
	action * actionHook(status&, deque<int>, int, int, vector<int>);
	actionTrie * child[10];
	deque<action*> fish;
	actionTrie * insert(int);
	actionTrie * insert(int, action*);
	void insert(action *, int);
	void insert(action *, string);
	unsigned int patternMatch(int[], bool[]);
private:
	vector<int> pattern;
};
#endif
