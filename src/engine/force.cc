#include "force.h"
#include <SDL/SDL.h>
#include <fstream>
#include <iostream>
#include <math.h>

using std::get;
using std::make_shared;
shared_ptr<force> makeForce(int t)
{
	shared_ptr<force> f;
	switch(t)
	{
		case 0: f = make_shared<globalForce>(); break;
		case 1: f = make_shared<linearDecay>(); break;
		case 2: f = make_shared<halfLifeDecay>(); break;
		case 3: f = make_shared<cutoffDecay>(); break;
		default: f = nullptr;  
	} 
	return f; 
}

tuple<SDL_Rect, SDL_Rect, float, bool> force::enforceBegin(const instance& a)
{
	SDL_Rect resultant;
	int midpoint, xDist, yDist;
	int directionX = 0, directionY = 0;
	float totalDist;
	if(a.current.facing == 1) midpoint = a.current.posX + a.current.facing*a.current.move->collision[a.current.frame].x + a.current.facing*a.collision.w/2;
	else midpoint = a.current.posX + a.current.facing*a.current.move->collision[a.current.frame].x + a.current.facing*a.collision.w/2 + a.collision.w%2;
	resultant.x = x; resultant.y = y;

	xDist = abs(midpoint - current.posX);
	yDist = abs(a.collision.y + a.collision.h/2 - current.posY);
	totalDist = sqrt(pow(xDist, 2) + pow(yDist, 2));

	if(!a.current.aerial) resultant.y = 0;

	if(midpoint > current.posX) directionX = 1;
	else if(midpoint < current.posX) directionX = -1;


	if(a.collision.y + a.collision.h/2 > current.posY) directionY = 1;
	else if(a.collision.y + a.collision.h/2 < current.posY) directionY = -1;

	bool stop = (totalDist < eventHorizon && eventHorizon > 0 && grip);
	SDL_Rect intermediate;
	intermediate.x = xDist; intermediate.y = yDist;
	intermediate.w = directionX; intermediate.h = directionY;

	return tuple<SDL_Rect, SDL_Rect, float, bool> {resultant, intermediate, totalDist, stop};
}

void force::enforceStop(instance& a, SDL_Rect& resultant)
{
	resultant.x = 0;
	resultant.y = 0;
	a.current.deltaX = 0; a.current.deltaY = 0;
	a.current.posX = current.posX - a.collision.w/2;
	a.current.posY = current.posY - a.collision.h/2;
	grip--;
	a.momentum.clear();
}

void globalForce::enforce(instance& a)
{
	auto r = enforceBegin(a);
	SDL_Rect resultant = get<0>(r);
	if (get<3>(r)) enforceStop(a, resultant);
	a.momentum.push_back(resultant);
};

void linearDecay::enforce(instance& a)
{
	auto r = enforceBegin(a);
	SDL_Rect resultant = get<0>(r);
	if (get<3>(r)) enforceStop(a, resultant);
	else
	{
		resultant.x *= get<1>(r).x * radius / get<2>(r);
		resultant.y *= get<1>(r).y * radius / get<2>(r);
		resultant.x *= get<1>(r).w;
		resultant.y *= get<1>(r).h;	
	}
	a.momentum.push_back(resultant);
}

void halfLifeDecay::enforce(instance& a)
{	
	auto r = enforceBegin(a);
	SDL_Rect resultant = get<0>(r);
	if (get<3>(r)) enforceStop(a, resultant);
	else
	{	
		resultant.x *= get<1>(r).x * radius / pow(get<2>(r), 2);
		resultant.y *= get<1>(r).y * radius / pow(get<2>(r), 2);
		resultant.x *= get<1>(r).w;
		resultant.y *= get<1>(r).h;
	}
	a.momentum.push_back(resultant);
}

void cutoffDecay::enforce(instance & a)
{
	auto r = enforceBegin(a);
	SDL_Rect resultant = get<0>(r);
	if (get<3>(r)) enforceStop(a, resultant);
	else
	{
		if(get<2>(r) > radius)
		{
			resultant.x = 0;
			resultant.y = 0;
		}
		else
		{
			resultant.x *= get<1>(r).w;
			resultant.y *= get<1>(r).h;
		}
	}
	a.momentum.push_back(resultant);
}
