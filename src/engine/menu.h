#ifndef MENU_INCLUDED
#define MENU_INCLUDED
#include <vector>
#include <string>
using std::vector;
using std::string;
class fightingGame;

class menu {
public:
	menu() {}
	menu(fightingGame*);
	void associate(fightingGame*);
	void pauseMenu();
	void draw();
	vector<string> labels;
	fightingGame * game;
	int cursor;
	void operator()();
	operator bool() const;
};
#endif
