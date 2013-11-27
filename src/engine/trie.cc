#include "trie.h"

actionTrie::actionTrie()
{
	for(int i = 0; i < 10; i++)
		child[i] = nullptr;
}

actionTrie::actionTrie(action * a, int p)
{
	for(int i = 0; i < 10; i++)
		child[i] = nullptr;
	pattern.push_back(p);
	fish.push_back(a);
}

actionTrie::actionTrie(action * a)
{
	for(int i = 0; i < 10; i++)
		child[i] = nullptr;
	pattern.push_back(0);
	fish.push_back(a);
}

void actionTrie::insert(action * b, string p)
{
	actionTrie * t = this;
	int q, pattern = 0;
	for(int i = p.length()-1; i > 0; i--){
		switch(p[i]){
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
			pattern += 1 << (p[i] - 'A');
			break;
		default:
			q = stoi(p.substr(i, 1));
			if(q > 10) q = q % 10;
			t = t->insert(q);
			break;
		}
	}
	t->insert(b, pattern);
}

void actionTrie::insert(action * b, int p)
{
	fish.push_back(b);
	pattern.push_back(p);
}

actionTrie * actionTrie::insert(int a, action * b)
{
	if(a < 10 && a >= 0){
		if(!child[a]) child[a] = new actionTrie(b);
		else child[a]->insert(b, 0);
		return child[a];
	}
	else return nullptr;
}

actionTrie * actionTrie::insert(int a)
{
	if(a < 10 && a >= 0) {
		if(!child[a]) child[a] = new actionTrie();
		return child[a];
	}
	else return nullptr;
}

actionTrie::~actionTrie()
{
	for(int i = 0; i < 9; i++){
		if(child[i] != nullptr){
			delete child[i];
			child[i] = nullptr;
		}
	}
}

action * actionTrie::actionHook(status &current, deque<int> inputBuffer, int i, int first, vector<int> buttons)
{
	actionTrie * test = nullptr;
	action * result = nullptr;
	int j;
	for(j = i; j < 30; j++){
		if(inputBuffer[j] < 0 || inputBuffer[j] > 9) test = child[5];
		else test = child[inputBuffer[j]];
		if(test != nullptr){
			if (first < 0) result = test->actionHook(current, inputBuffer, j, j, buttons);
			else result = test->actionHook(current, inputBuffer, j, first, buttons);
			if(result != nullptr) return result;
		}
	}
	if(fish.size() != 0){
		for(unsigned int k = 0; k < fish.size(); k++){
			if(fish[k]){
				if(fish[k]->activate(current, buttons, pattern[k], i, first)){
					return fish[k];
				}
			}
		}
	}
	return nullptr;
}
