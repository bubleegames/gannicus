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
	void (*resolve)(menu*);
	void draw();
	void toggle();
	vector<string> labels;
	fightingGame * game;
	int cursor;
	bool on;
	void operator()();
	operator bool() const;
};
#endif
