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
	fish.push_back(actionHandle(a,p));
}

bool actionHandle::operator==(const action *a)
{
	return a == target;
}

actionTrie::actionTrie(action * a)
{
	for(int i = 0; i < 10; i++)
		child[i] = nullptr;
	fish.push_back(actionHandle(a, 0));
}

actionTrie * actionTrie::search(int s)
{
	if(child[s%10]){
		if(s < 10) return child[s%10];
		else return child[s%10]->search(s/10);
	} else return nullptr;
}

void actionTrie::remove(action *s)
{
	for(unsigned int i = 0; i < fish.size(); i++){
		if(fish[i] == s){
			fish.erase(fish.begin()+i);
			i--;
		}
	}
	for(actionTrie *i:child)
		if(i) i->remove(s);
}

void actionTrie::insert(action * b, string p)
{
	actionTrie * t = this;
	int q, pat = 0;
	for(int i = p.length()-1; i > 0; i--){
		switch(p[i]){
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
			pat += 1 << (p[i] - 'A');
			break;
		default:
			q = stoi(p.substr(i, 1));
			if(q > 10) q = q % 10;
			t = t->insert(q);
			break;
		}
	}
	t->insert(b, pat);
}

void actionTrie::insert(action * b, int p)
{
	fish.push_back(actionHandle(b, p));
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
			if(fish[k].target){
				if(fish[k].target->activate(current, buttons, fish[k].pattern, i, first)){
					return fish[k].target;
				}
			}
		}
	}
	return nullptr;
}
