#include "menu.h"
#include "interface.h"
menu::menu(fightingGame* g) : cursor(0), on(false)
{
	associate(g);
}

void menu::associate(fightingGame *g)
{
	game = g;
}

void menu::operator()()
{
	resolve(this);
}

menu::operator bool() const
{
	return on;
}

void menu::toggle()
{
	on = !on;
}
