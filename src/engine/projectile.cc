#include "interface.h"

projectile::projectile(string directory, string file)
{
	head = new actionTrie;
	neutral = nullptr;
	build(directory, file);
}

void pet::init(status& current)
{
	character::init(current);
}

pet::pet(string directory, string file)
{
	head = new actionTrie;
	neutral = nullptr;
	build(directory, file);
}

void pet::build(string directory, string file)
{
	projectile::build(directory, file);
}

void projectile::build(string directory, string file)
{
	getName(directory.c_str(), file.c_str());
	die = mandateMove("die");
	avatar::build(directory.c_str(), file.c_str());
	head->insert(die, 0);
	lifespan = -1;
}

int projectile::acceptTarget(action * c, int f)
{
	if(c->stats[c->calcCurrentHit(f)].hitsProjectile || c->stats[c->calcCurrentHit(f)].killsProjectile || c->stats[c->calcCurrentHit(f)].turnsProjectile)
		return 1;
	return 0;
}

hStat projectile::pollStats(status &current)
{
	hStat s = current.move->pollStats(current.frame, current.counter);
	s.isProjectile = true;
	if(s.pause == -1 && !s.ghostHit)
		s.pause = 5;
	return s;
}

void projectile::init(status& current)
{
	current.meter[0] = 600;
	current.meter[1] = 0;
	current.meter[2] = 1;
	current.meter[3] = 1;
}

void projectile::processMove(action * m)
{
	if(neutral == die || neutral == nullptr){ 
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

int pet::takeHit(status &current, hStat &s, int blockType, int &hitType)
{
	if(current.move->takeHit(s, blockType, current) == 1)
		return projectile::takeHit(current, s, blockType, hitType);
	else
		return character::takeHit(current, s, blockType, hitType);
}

int projectile::takeHit(status &current, hStat &s, int blockType, int &hitType)
{
	if(s.killsProjectile || current.move->hittable){ 
		die->execute(current);
		current.move = die;
		return 1;
	} else return 0;
}

bool projectile::death(status &current)
{
	if(current.move == die){
		if(current.frame == current.move->frames - 1){
			return true;
		}
	}
	if(current.move != die && lifespan > 0 && current.age > lifespan){
		current.move = die;
	}
	return false;
}

