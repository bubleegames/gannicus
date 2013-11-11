#include "yellow.h"
yellow::yellow(){
	head = new actionTrie;
	airHead = new actionTrie;
	build("Yellow", "Yellow");
}

void yellow::resetAirOptions(vector<HUDMeter<int>>& meter)
{
	meter[2].value = 1;
	meter[3].value = 2;
}

vector<HUDMeter<int>> yellow::generateMeter()
{
	vector<HUDMeter<int>> meter = character::generateMeter();
	meter.push_back(HUDMeter<int>(360));
	meter[5].y = 1.0/90.0 * 88.5;
	meter[5].x = .275;
	meter[5].h = 1.0/90.0;
	meter[5].w = .45;
	meter[5].B = 0; meter[5].A = 1.0;

	return meter;
}

void yellow::init(status &current)
{
	character::init(current);
	current.meter[5].value = 0;
}

void yellow::tick(status &current)
{
	character::tick(current);
	if(current.meter[5].value > 0){
		if(current.meter[4].value < 2) current.meter[4].value = 2;
		current.meter[5].value--;
		if(current.meter[5].value == 0 && current.mode & 1){ 
			current.meter[5].value = -360;
			current.mode = 0;
		}
	}
}

void yellow::step(status &current)
{
	if(current.meter[5].value < 0) current.meter[5].value++;
	character::step(current);
}

action * yellow::createMove(string key)
{
	tokenizer t(key, " \t-@?_%&$!\n");
	string token = t();
	action * m;
	switch(key[0]){
	case '$':
		m = new flashStep(name, token);
		break;
	case '&':
		m = new flashSummon(name, token);
		break;
	default:
		m = character::createMove(key);
		break;
	}
	if(m->typeKey == '0') m->typeKey = key[0];
	return m;
}

vector<HUDMeter<int>> yellow::drawMeters(int ID, status &current)
{
	vector<HUDMeter<int>> ret = character::drawMeters(ID, current);
	ret.push_back(current.meter[5]);
	ret[5].R = 1.0;
	ret[5].G = ret[5].value >= 0 ? 1.0 : 0;
	ret[5].value *= ret[5].value >= 0 ? 2.0 / 3.0 : 0.0;
	return ret;
}

int yellow::takeHit(status& current, hStat & s, int blockType, int &hitType)
{
	int x = character::takeHit(current, s, blockType, hitType);
	if(hitType == -2) current.meter[5].value = 21;
	if(hitType == 1 && current.meter[5].value > 0) current.meter[5].value = 0;
	return x;
}

flashStep::flashStep() {}
flashSummon::flashSummon() {}

flashStep::flashStep(string dir, string file)
{
	build(dir, file);
}

flashSummon::flashSummon(string dir, string file)
{
	build(dir, file);
}

void flashSummon::zero()
{
	flashMeterGain = 0;
	action::zero();
}

bool flashSummon::setParameter(string buffer)
{
	tokenizer t(buffer, "\t:+\n");
	if(t() == "FlashGain"){
		flashMeterGain = stoi(t("\t:\n"));
		return 1;
	} else return action::setParameter(buffer);
}

bool flashStep::check(const status &current)
{
	if(current.meter[5].value < 1) return 0;
	else return action::check(current);
}

bool flashSummon::check(const status &current)
{
	if(current.meter[5].value < 0) return 0;
	int temp = cost;
	if(current.mode & 1) cost = 0;
	bool ret = action::check(current);
	cost = temp;
	return ret;
}

action * flashSummon::execute(status &current)
{
	current.mode = (current.mode & 1) ? 0 : 1;
	int temp = cost;
	if(current.mode & 1) cost = 0;
	cost = temp;
	return action::execute(current);
}

action * flashStep::execute(status &current)
{
	current.meter[5].value -= 10;
	if(current.meter[5].value > 540) current.meter[5].value = 540;
	else if(current.meter[5].value < 0) current.meter[5].value = -360;
	return action::execute(current);
}

void flashSummon::step(status &current)
{
	if(current.mode & 1){
		if(current.frame == frames - 1) current.meter[5].value = 0;
	} else current.meter[5].value += flashMeterGain / frames + 1;
	if(current.meter[5].value > 540) current.meter[5].value = 540;
	action::step(current);
}
