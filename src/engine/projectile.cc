#include "interface.h"
projectile::projectile(const char* directory, const char* file)
{
	head = new actionTrie;
	neutral = NULL;
	build(directory, file);
}

void projectile::build(const char* directory, const char* file)
{
	getName(directory, file);

	char buffer[101];

	sprintf(buffer, "%s/die", name);
	die = new action(buffer);
	head->insert(die, 0);
	avatar::build(directory, file);
	lifespan = -1;
}

int projectile::acceptTarget(action * c, int f)
{
	if(c->stats[c->calcCurrentHit(f)].hitsProjectile || c->stats[c->calcCurrentHit(f)].killsProjectile || c->stats[c->calcCurrentHit(f)].turnsProjectile)
		return 1;
	return 0;
}

void projectile::pollStats(hStat & s, status &current, bool CH)
{
	s.isProjectile = true;
	current.move->pollStats(s, current.frame, CH);
}

void projectile::init(std::vector<int>& meter)
{
	meter[0] = 600;
	meter[1] = 0;
	meter[2] = 1;
	meter[3] = 1;
}

void projectile::processMove(action * m)
{
	if(neutral == NULL){ 
		neutral = m;
	}
	avatar::processMove(m);
	if(m->dies){ 
		m->feed(die, 2, m->hits-1);
		m->feed(die, 0, 0);
	}
}

bool projectile::turn(int &ID)
{
	ID = ID % 2 + 1;
	return 1;
}

int projectile::takeHit(status &current, hStat &s, int blockType, int &hitType, std::vector<int>& meter)
{
	if(s.killsProjectile || current.move->hittable){ 
		die->execute(current.move, meter, current.frame, current.connect, current.hit);
		current.move = die;
		return 1;
	} else return 0;
}

bool projectile::death(action *& cMove, int f, int counter)
{
	if(cMove == die){
		if(f == cMove->frames - 1){
			return true;
		}
	}
	if(cMove != die && lifespan > 0 && counter > lifespan){
		cMove = die;
	}
	return false;
}

