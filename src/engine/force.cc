#include "force.h"
#include <SDL/SDL.h>
#include <fstream>
#include <iostream>
#include <math.h>
void force::enforce(instance * a)
{
	SDL_Rect resultant;
	int midpoint, xDist, yDist;
	int directionX = 0, directionY = 0;
	float totalDist;
	if(a->current.facing == 1) midpoint = a->current.posX + a->current.facing*a->current.move->collision[a->current.frame].x + a->current.facing*a->collision.w/2;
	else midpoint = a->current.posX + a->current.facing*a->current.move->collision[a->current.frame].x + a->current.facing*a->collision.w/2 + a->collision.w%2;
	resultant.x = x; resultant.y = y; resultant.w = 0; resultant.h = 0;
	xDist = abs(midpoint - posX);
	yDist = abs(a->collision.y + a->collision.h/2 - posY);
	totalDist = sqrt(pow(xDist, 2) + pow(yDist, 2));
	if(!a->current.aerial) resultant.y = 0;
	if(midpoint > posX) directionX = 1;
	else if(midpoint < posX) directionX = -1;
	if(a->collision.y + a->collision.h/2 > posY) directionY = 1;
	else if(a->collision.y + a->collision.h/2 < posY) directionY = -1;
	if(totalDist < eventHorizon && eventHorizon > 0 && grip){
		resultant.x = 0;
		resultant.y = 0;
		a->current.deltaX = 0; a->current.deltaY = 0;
		a->current.posX = posX - a->collision.w/2;
		a->current.posY = posY - a->collision.h/2;
		grip--;
		a->momentum.clear();
	} else {
		switch(type){
		case 0:
			break;
		case 1:
			resultant.x *= xDist * radius / totalDist;
			resultant.y *= yDist * radius / totalDist;
			resultant.x *= directionX;
			resultant.y *= directionY;
			break;
		case 2:
			resultant.x *= xDist * radius / pow(totalDist, 2);
			resultant.y *= yDist * radius / pow(totalDist, 2);
			resultant.x*= directionX;
			resultant.y*= directionY;
			break;
		case 3:
			if(totalDist > radius){
				resultant.x = 0;
				resultant.y = 0;
			}
			resultant.x *= directionX;
			resultant.y *= directionY;
			break;
		default:
			return;
		}
	}
	a->momentum.push_back(resultant);
}

bool force::validate(instance *a){
	cout << a->pick()->name << '\n';
	return (ID == a->ID) ? false : (effectCode & 1);
}

bool force::validate(player *a){
	cout << "Daquiri!\n";
	return (ID == a->ID) ? false : (effectCode & 2);
}
