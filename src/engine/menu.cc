#include "menu.h"
#include "interface.h"
menu::menu(fightingGame* g)
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
		if(cursor > 4) cursor = 1;
		else if(cursor < 1) cursor = 4;
		for(unsigned int i = 0; i < game->currentFrame[j].buttons.size()-1; i++){
			if(game->currentFrame[j].buttons[i] == 1){
				switch(cursor){
				case 1:
					cursor = 0;
					break;
				case 2:
					for(unsigned int i = 0; i < game->P.size(); i++){
						game->P[i]->characterSelect(nullptr);
						game->select[i] = 0;
						game->things[i]->current.meter.clear();
					}
					Mix_HaltMusic();
					Mix_FreeMusic(game->matchMusic);
					//Mix_PlayChannel(3, announceSelect, 0);
					game->matchInit();
					break;
				case 3:
					Mix_HaltMusic();
					Mix_FreeMusic(game->matchMusic);
					game->gameover = 1;
					break;
				}
				j = 2;
				break;
			}
		}
		if(game->currentFrame[j].n.raw.Start && !game->counter[j]) cursor = 0;
	}
}

menu::operator bool() const
{
	return cursor != 0;
}
