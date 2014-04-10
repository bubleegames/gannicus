#include "status.h"
#include "player.h"
status::status()
{
	deltaX = 0;
	deltaY = 0;
	delay = 0;
	lCorner = 0;
	rCorner = 0;
	frame = 0;
	connect = 0;
	counter = 0;
	comboState = 0;
	absorbedHits = 0;
	hit = 0;
	reversalTimer = 0;
	move = nullptr;
	bufferedMove = nullptr;
	reversal = nullptr;
	freeze = 0;
	aerial = false;
	dead = false;
	elasticX = false;
	elasticY = false;
	slide = false;
	stick = false;
	rebound = false;
	hover = 0;
	opponent = nullptr;
}

status::status(const status & o) 
{
	posX = o.posX; posY = o.posY;
	drawX = o.drawX; drawY = o.drawY;
	facing = o.facing;
	deltaX = o.deltaX; deltaY = o.deltaY; 
	frame = o.frame;
	freeze = o.freeze;
	reversalTimer = o.reversalTimer;
	aerial = o.aerial;
	rCorner = o.rCorner; lCorner = o.lCorner;
	move = o.move; bufferedMove = o.bufferedMove; reversal = o.reversal;
	connect = o.connect; hit = o.hit; counter = o.counter;
	momentum = o.momentum;
	dead = o.dead;
	throwInvuln = o.throwInvuln;
	reversalFlag = o.reversalFlag;
	meter = o.meter;
	cooldowns = o.cooldowns;
	offspring = o.offspring;
	collision = o.collision;
	comboState = o.comboState;
	hitbox = o.hitbox;
	hitreg = o.hitreg;
	age = o.age;
	dead = o.dead;
	mode = o.mode;
	absorbedHits = o.absorbedHits;
	elasticX = o.elasticX;
	elasticY = o.elasticY;
	slide = o.slide;
	stick = o.stick;
	delay = o.delay;
	rebound = o.rebound;
	hover = o.hover;
	opponent = o.opponent;
}

SDL_Rect status::prox() const
{
	return prox(opponent);
}

SDL_Rect status::prox(instance* a) const
{
	return {0, 0, (Uint16)abs(posX - a->current.posX), (Uint16)abs(posY - a->current.posY)};
}

int status::cancelState() const
{
	if(!move) return 0;
	if(hit > move->hits) return 0;
	if(connect > move->hits) return 0;
	if(connect < 0) return 0;
	return move->state[connect].i;
//		+ (hit == connect ? move->stats[hit].hitState.i : 0);
}

