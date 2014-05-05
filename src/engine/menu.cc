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
	for(unsigned int j = 0; j < game->p.size(); j++){
		if(game->currentFrame[j].axis[0] && !game->counter[j]){
			cursor--;
			game->counter[j] = 10;
		} else if(game->currentFrame[j].axis[1] && !game->counter[j]){ 
			cursor++;
			game->counter[j] = 10;
		}
		if(cursor < 0) cursor = labels.size() - 1;
		else if((unsigned int)cursor >= labels.size()) cursor = 0;
		for(unsigned int i = 0; i < game->currentFrame[j].buttons.size()-1; i++){
			if(game->currentFrame[j].buttons[i] == 1){
				runResolver(this);
				j = 2;
				break;
			}
		}
		if(game->currentFrame[j].n.raw.Start && !game->counter[j]){
			toggle();
			game->counter[j] = 10;
		}
	}
}

menu::operator bool() const
{
	return on;
}

void menu::toggle()
{
	on = !on;
}
