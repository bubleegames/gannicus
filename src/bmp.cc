#include <iostream>
#include <stdio.h>
#include <fstream>
#include "engine/interface.h"
#include <cstring>
using std::to_string;
int main(int argc, char* argv[])
{
	SaltAndBone game;
	game.screenInit();
	game.createPlayers(1);
	game.startGame();
	game.demo = true;
	game.killTimer = true;
	game.numRounds = 1;
	game.single = true;
	game.screenShot = true;
	for(unsigned int i = 0; i < game.characterManifest.size(); i++){
		if(game.characterManifest[i] == argv[1]){
			game.P[0]->characterSelect(game.generateCharacter(i));
			game.select[0] = true;
			break;
		}
	}
	for(action *i:game.P[0]->pick()->moveList){
		game.matchInit();
		game.roundInit();
		game.P[0]->current.move = i;
		while(game.P[0]->current.move == i){
			for(instance *i:game.things) i->updateRects();
			game.resolveThrows();
			game.doSuperFreeze();
			for(instance *i:game.things) i->updateRects();
			game.resolvePhysics();
			game.resolveCamera();
			game.draw();
			game.saveScreenshot(string("dump/"+game.P[0]->current.move->name+"#"+to_string(game.P[0]->current.frame)+".bmp"));
			game.cleanup();
		}
	}
	game.gameover = true;
}

