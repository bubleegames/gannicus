#include "action.h"
#include "player.h"
#include "tokenizer.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include "force.h"
using std::ifstream;
using std::ofstream;
using std::to_string;
using std::make_shared;
action::action() {}

action::action(string dir, string file)
{
	build(dir, file);
}

action::~action()
{
	if(!this) return;
	if(next) delete next;
}

void action::zero()
{
	typeKey = '0';
	frames = 0;
	hits = 0;
	unique = false;
	freezesProjectiles = true;
	delayFrame = -1;
	delayMax = -1;
	delayCheck = 0;
	disables = 0;
	DIB = 0; DIF = 0; DIU = 0; DID = 0;
	resetJumpOptions = false;
	requiresFreeze = false;
	subsumedFreeze = false;
	freezeAgnostic = false;
	selfChain = false;
	cooldown = 0;
	timeDilation = 0;
	collision.clear();
	hitbox.clear();
	hitreg.clear();
	visibleBox.clear();
	delta.clear();
	tempNext.clear();
	tempFollowup.clear();
	tempOnConnect.clear();
	tempAttempt.clear();
	tempPayload.clear();
	tempRiposte.clear();
	tempOnHold.clear();
	tempParticle.clear();
	restrictedMode = 0;
	requiredMode = 0;
	activateMode = 0;
	removeMode = 0;
	particleX = 0;
	particleY = 0;
	particleSpawn = -1;
	offX = 0;
	offY = 0;
	linkable = 0;
	guardType = 0;
	holdCheck = -1;
	holdFrame = -1;
	xRequisite = 0;
	yRequisite = 0;
	stunMin = 0;
	stunMax = 0;
	stop = 0;
	hits = 0;
	throwinvuln = 0;
	minHold = 1;
	maxHold = 1;
	crouch = 0;
	armorStart = -1; armorLength = 0;
	armorHits = 0;
	guardStart = -1; guardLength = 0;
	freezeFrame = -1; freezeLength = 0;
	blockState.i = 0;
	cost = 0;
	dies = 0;
	fch = 0;
	track = false;
	flip = -1;
	armorCounter = 0;
	distortSpawn = -1;
	distortion = nullptr;
	hidesMeter = 0;
	displaceFrame = -1;
	displaceX = 0;
	displaceY = 0;
	snapToFrame = -1;
	snapToX = 0;
	snapToY = 0;
	soundClip = nullptr;
	countersProjectile = true;
	next = nullptr;
	attempt = nullptr;
	riposte = nullptr;
	basis.move = nullptr;
	onHold = nullptr;
	hittable = false;
	modifier = false;
	payload = nullptr;
	spawnFrame = 0;
	spawnTrackY = 0; spawnTrackX = 0;
	spawnTrackFloor = 0;
	spawnPosY = 0; spawnPosX = 0;
	werf = 0;
	carryPosX = 0; carryPosY = 0;
	lifespan = -1;
	killFlags = 0;
	allegiance = 1;
	followStart = -1;
	followEnd = -1;
	followXRate = 0;
	followYRate = 0;
}

void action::generate(string directory, string name)
{
	tokenizer t(name, "%");
	if(name[0] == '%') payload = new pet(directory, t());
	else payload = new projectile(directory, name);
	if(disables) payload->captures = disables;
	if(lifespan) payload->lifespan = lifespan;
	if(killFlags) payload->killFlags = killFlags;
}

instance * action::spawn()
{
	instance * ret = nullptr;
	if(payload) ret = new instance(payload);
	return ret;
}

int action::arbitraryPoll(int q, int f)
{
	switch(q){
	case 2:
		if(f == freezeFrame) return freezeLength;
		else break;
	case 31:
		return timeDilation;
	case 32:
		return freezesProjectiles;
	case 28:
		if(werf && f == 0) return 1;
		break;
	case 27:
		return carryPosX;
	case 26:
		return carryPosY;
	case 50:
		if(f == spawnFrame) return 1;
		else break;
	case 51:
		if(spawnTrackX) return 1;
		else break;
	case 52:
		if(spawnTrackY) return 1;
		else break;
	case 53:
		if(spawnTrackFloor) return 1;
		else break;
	case 54:
		return spawnPosX;
	case 55:
		return spawnPosY;
	case 56:
		return allegiance;
	}
	return 0;
}

void action::build(string dir, string n)
{
	zero();
	fileName = n;
	ifstream read;
	char buffer[1024];
	buffer[0] = '\0';

	read.open("content/characters/"+dir+"/"+fileName+".mv");
	if(read.fail()){
		//printf("Move %s/%s not found. Skipping\n", dir.c_str(), fileName.c_str());
		null = true;
		return;
	} else null = false;

	do {
		read.getline(buffer, 1000);
	} while (setParameter(buffer));

	for(int i = 0; i < frames; i++){
		do {
			read.getline(buffer, 1000);
			parseRect(buffer);
		} while (buffer[0] == '$');
		if(hitbox.size() < (unsigned int)i+1) hitbox.push_back(vector<SDL_Rect>(0));
		if(hitreg.size() < (unsigned int)i+1) hitreg.push_back(vector<SDL_Rect>(0));
		if(delta.size() < (unsigned int)i+1) delta.push_back(vector<SDL_Rect>(0));
		if(visibleBox.size() < (unsigned int)i+1) visibleBox.push_back(vector<SDL_Rect>(0));
		if(collision.size() < (unsigned int)i+1){ 
			SDL_Rect a;
			collision.push_back(a);
		}
	}
	read.close();
}

bool action::parseRect(string buffer)
{
	switch(buffer[1]){
	case 'C':
		collision.push_back(aux::defineRectArray(buffer.substr(2))[0]);
		return 1;
	case 'R':
		hitreg.push_back(aux::defineRectArray(buffer.substr(2)));
		return 1;
	case 'D':
		delta.push_back(aux::defineRectArray(buffer.substr(2)));
		return 1;
	case 'A':
		hitbox.push_back(aux::defineRectArray(buffer.substr(2)));
		return 1;
	case 'V':
		visibleBox.push_back(aux::defineRectArray(buffer.substr(2)));
		return 1;
	}
	return 0;
}

void action::loadMisc(string dir)
{
	SDL_Surface *temp;
	for(int i = 0; i < frames; i++){
		temp = aux::load_image("content/characters/" + dir + "/" + fileName + "#" + to_string(i) + ".png");
		if(!temp){
			width.push_back(0);
			height.push_back(0);
			sprite.push_back(0);
		} else {
			width.push_back(temp->w);
			height.push_back(temp->h);
			sprite.push_back(aux::surface_to_texture(temp));
		}
		SDL_FreeSurface(temp);
	}
//	soundClip = Mix_LoadWAV(string("content/characters/"+dir+"/"+fileName+".ogg").c_str());
}

bool action::setParameter(string param)
{
	tokenizer t(param, "\t:+\n");
	if(t() == "Name"){;
		name += t();
		return true;
	} else if (t.current() == "+Mode") {
		activateMode += stoi(t("\t: \n"));
		return true;
	} else if (t.current() == "-Mode") {
		removeMode += stoi(t("\t: \n"));
		return true;
	} else if (t.current() == "RestrictedMode") {
		restrictedMode = stoi(t("\t: \n"));
		return true;
	} else if (t.current() == "RequiredMode") {
		requiredMode = stoi(t("\t: \n"));
		return true;
	} else if (t.current() == "TimeDilation") {
		timeDilation = stoi(t("\t:\n"));
		return true;
	} else if (t.current() == "SnapTo") {
		snapToFrame = stoi(t("\t:\n"));
		snapToX = stoi(t());
		snapToY = stoi(t());
		return true;
	} else if (t.current() == "Displace") {
		displaceFrame = stoi(t("\t:\n"));
		displaceX = stoi(t());
		displaceY = stoi(t());
		return true;
	} else if (t.current() == "Buffer") {
		tolerance = stoi(t("\t: \n"));
		activation = stoi(t());
		return true;
	} else if(t.current() == "Proximity"){
		xRequisite = stoi(t("\t: \n"));
		yRequisite = stoi(t());
		return true;
	} else if (t.current() == "Cooldown") {
		cooldown = stoi(t("\t: \n"));
		return true;
	} else if (t.current() == "DirectionalInfluence") {
		DIU = stoi(t("\t: \n"));
		DID = stoi(t());
		DIB = stoi(t());
		DIF = stoi(t());
		return true;
	} else if (t.current() == "Offset") {
		offX = stoi(t("\t: \n"));
		offY = stoi(t());
		return true;
	} else if (t.current() == "Hold") {
		minHold = stoi(t("\t: \n-"));
		maxHold = stoi(t());
		return true;
	} else if (t.current() == "Disables") {
		disables = 0;
		for(char c : t()){
			switch(c){
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
				disables += 1 << (c - 'A');
				break;
			}
		}
		return true;
	} else if (t.current() == "Delayable") {
		delayCheck = 0;
		for(char c : t()){
			switch(c){
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
				delayCheck += 1 << (c - 'A');
				break;
			}
		}
		delayFrame = stoi(t("\t: \n-"));
		delayMax = stoi(t()) - delayFrame;
		return true;
	} else if (t.current() == "Properties") {
		parseProperties(param, false);
		return true;
	} else if (t.current() == "Counterhit") {
		parseProperties(param, true);
		return true;
	} else if (t.current() == "Hits") {
		hits = stoi(t("\t: \n"));
		if(hits > 0){
			stats = vector<hStat> (hits);
			CHStats = vector<hStat> (hits);
			tempOnConnect = vector <string> (hits);
			for(unsigned int i = 0; i < stats.size(); i++) stats[i].hitState.i = 0;
			for(unsigned int i = 0; i < CHStats.size(); i++) CHStats[i].hitState.i = 0;
		}
		state = vector<cancelField> (hits+1);
		gain = vector<int> (hits+1);
		for(unsigned int i = 0; i < state.size(); i++) state[i].i = 0;
		for(int &i:gain) i = 0;
		return true;
	} else if (t.current() == "Riposte") {
		tempRiposte = t("\t: \n");
		return true;
	} else if (t.current() == "OnHold") {
		holdFrame = stoi(t("\t:- \n"));
		holdCheck = 0;
		for(char c : t()){
			switch(c){
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
				holdCheck += 1 << (c - 'A');
				break;
			}
		}
		tempOnHold = t();
		return true;
	} else if (t.current() == "Distort") {
		int type = stoi(t("\t: \n"));
		distortion = makeForce(type);
		distortSpawn = stoi(t("\t:- \n"));
		distortion->type = type; //This is kind of stupid. But we need to be able to make copies of the right derived force somehow...
		distortion->length = stoi(t());
		distortion->length -= distortSpawn;
		distortion->x = stoi(t("\t: \n"));
		distortion->y = stoi(t());

		return true;
	} else if (t.current() == "Radius") {
		if(!distortion) return true;
		distortion->radius = stoi(t());
		return true;
	} else if (t.current() == "EventHorizon") {
		distortion->eventHorizon = stoi(t("\t: \n"));
		distortion->grip = stoi(t());
		return true;
	} else if (t.current() == "Attracts") {
		distortion->ID = stoi(t("\t: \n"));
		distortion->effectCode = stoi(t());
		return true;
	} else if (t.current() == "Next") {
		tempNext = t("\t: \n");
		return true;
	} else if (t.current() == "Flip") {
		flip = stoi(t("\t: \n"));
		return true;
	} else if (t.current() == "Attempt") {
		int s = stoi(t("\t: \n-")),
		    e = stoi(t());
		attemptSpan = span<int> (s,e);
		tempAttempt = t();
		return true;
	} else if (t.current() == "Reflector") {
		int s = stoi(t("\t: \n-")),
		    e = stoi(t());
		reflectorSpan = span<int> (s,e);
		return true;
	} else if (t.current() == "Connect") {
		int x = stoi(t("\t: \n"));
		tempOnConnect[x] = t();
		return true;
	} else if (t.current() == "Blocks") {
		blockState.i = stoi(t("\t: \n"));
		return true;
	} else if (t.current() == "Check") {
		allowed.i = stoi(t("\t: \n"));
		return true;
	} else if (t.current() == "Cost") {
		string temp = t("\t: \n");
		if(temp[0] == '(') std::cout << stoi(temp);
		cost = stoi(temp);
		return true;
	} else if (t.current() == "Frames") {
		frames = stoi(t("\t: \n"));
		int startup, countFrames = -1;
		if(hits > 0) {
			totalStartup = vector<int> (hits);
			active = vector<int> (hits);
		}
		for(int i = 0; i < hits; i++){
			startup = stoi(t());
			countFrames += startup;
			totalStartup[i] = countFrames;
			active[i] = stoi(t());
			countFrames += active[i];
		}
		return true;
	} else if (t.current() == "State") {
		for(int i = 0; i < hits+1; i++){
			state[i].i = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "DiesOn"){
		for (char c : t()) {
			switch(c){
			case 'h': 
				killFlags += 1;
				break;
			case 'b':
				killFlags += 2;
				break;
			case 'i':
				killFlags += 4;
				break;
			case 'p':
				killFlags += 8;
				break;
			case 'k':
				killFlags += 16;
				break;
			}
		}
		return true;
	} else if (t.current() == "Throw"){
		for (char c : t()) {
			if(c == 'G') werf += 1;
			if(c == 'A') werf += 2;
		}
		return true;
	} else if (t.current() == "Position"){
		carryPosX = stoi(t());
		carryPosY = stoi(t("\t: \n"));
		return true;
	} else if (t.current() == "HitAllows") {
		for(int i = 0; i < hits; i++){
			stats[i].hitState.i = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Damage") {
		for(int i = 0; i < hits; i++){
			if(param[0] == '+')
				CHStats[i].damage = stoi(t("\t: \n"));
			else stats[i].damage = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Connects") {
		for(int i = 0; i < hits; i++){
			if(param[0] == '+')
				CHStats[i].connect = stoi(t("\t: \n"));
			else stats[i].connect = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Chip") {
		for(int i = 0; i < hits; i++){
			stats[i].chip = stoi(t( "\t: \n"));
		}
		return true;
	} else if (t.current() == "Prorate") {
		for(int i = 0; i < hits; i++){
			if(param[0] == '+')
				CHStats[i].prorate = stof(t("\t: \n"));
			else stats[i].prorate = stof(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Push") {
		for(int i = 0; i < hits; i++){
			if(param[0] == '+') 
				CHStats[i].push = stoi(t("\t: \n"));
			else stats[i].push = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Lift") {
		for(int i = 0; i < hits; i++){
			if(param[0] == '+')
				CHStats[i].lift = stoi(t("\t: \n"));
			else stats[i].lift = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Float") {
		for(int i = 0; i < hits; i++){
			if(param[0] == '+')
				CHStats[i].hover = stoi(t("\t: \n"));
			else stats[i].hover = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Blowback") {
		for(int i = 0; i < hits; i++){
			if(param[0] == '+')
				CHStats[i].blowback = stoi(t("\t: \n"));
			else stats[i].blowback = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Stun") {
		for(int i = 0; i < hits; i++){
			if(param[0] == '+')
				CHStats[i].stun = stoi(t("\t: \n"));
			else {
				stats[i].stun = stoi(t("\t: \n"));
				CHStats[i].stun = (stats[i].stun - 5) / 2;
			}
		}
		return true;
	} else if (t.current() == "Pause") {
		for(int i = 0; i < hits; i++){
			if(param[0] == '+')
				CHStats[i].pause = stoi(t("\t: \n"));
			else {
				stats[i].pause = stoi(t("\t: \n"));
			}
		}
		return true;
	} else if (t.current() == "Launch") {
		for(int i = 0; i < hits; i++){
			if(param[0] == '+')
				CHStats[i].initialLaunch = stoi(t("\t: \n"));
			else{
				stats[i].initialLaunch = stoi(t("\t: \n"));
				CHStats[i].initialLaunch = 10;
			}
		}
		return true;
	} else if (t.current() == "Untech") {
		for(int i = 0; i < hits; i++){
			if(param[0] == '+')
				CHStats[i].untech = stoi(t("\t: \n"));
			else{
				stats[i].untech = stoi(t("\t: \n"));
				CHStats[i].untech = 10;
			}
		}
		return true;
	} else if (t.current() == "Blockable") {
		for(int i = 0; i < hits; i++){
			stats[i].blockMask.i = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Gain") {
		for(int i = 0; i < hits+1; i++){
			gain[i] = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Autoguard") {
		guardStart = stoi(t("\t: \n-"));
		guardLength = stoi(t());
		guardLength = guardLength - guardStart;
		return true;
	} else if (t.current() == "BearStun") { //RARRRRRR
		stunMin = stoi(t("\t: \n-"));
		stunMax = stoi(t());
		return true;
	} else if (t.current() == "Followup") {
		followupSpan = span<int> (totalStartup[hits-1] + active[hits-1], frames);
		tempFollowup.push_back(t("\n:"));
		return true;
	} else if (t.current() == "GuardType") {
		guardType = stoi(t("\t: \n"));
		return true;
	} else if (t.current() == "Follow") {
		followStart = stoi(t("\t: \n-"));
		followEnd = stoi(t()); 
		followXRate = stoi(t());
		followYRate = stoi(t());
		return true;
	} else if (t.current() == "SuperFreeze"){
		freezeFrame = stoi(t("\t: \n-")); 
		freezeLength = stoi(t()); 
		freezeLength = freezeLength - freezeFrame;
		return true;
	} else if (t.current() == "Armor") {
		armorStart = stoi(t("\t: \n-")); 
		armorLength = stoi(t()); 
		armorLength = armorLength - armorStart;
		return true;
	} else if (t.current() == "MaxArmor") {
		armorHits = stoi(t("\t: \n-")); 
		return true;
	} else if(t.current() == "SpawnPosition"){
		spawnPosX = stoi(t("\t: \n"));
		spawnPosY = stoi(t());
		return true;
	} else if(t.current() == "Track"){
		for(char c : t("\t: \n")){
			switch(c){
			case 'x':
				spawnTrackX = true;
				break;
			case 'y':
				spawnTrackY = true;
				break;
			case 'f':
				spawnTrackFloor = true;
				break;
			}
		}
		return true;
	} else if(t.current() == "SpawnsOn"){
		spawnFrame = stoi(t("\t: \n"));
		return true;
	} else if(t.current() == "Lifespan"){
		lifespan = stoi(t("\t: \n"));
		return true;
	} else if(t.current() == "Allegiance"){
		allegiance = stoi(t("\t: \n"));
		return true;
	} else if(t.current() == "Payload"){
		tempPayload = t("\t: \n");
		return true;
	} else if(t.current() == "Particle"){
		tempParticle = t("\t: \n");
		particleSpawn = stoi(t("\t: \n"));
		particleX = stoi(t());
		particleY = stoi(t());
		return true;
	} else return 0;
}

void action::parseProperties(string properties, bool counter)
{
	int ch = 0;
	unsigned int i = 0;
	while(properties[i++] != ':'); i++;
	for(; i < properties.size(); i++){
		switch(properties[i]){
		case '8':
			freezesProjectiles = false;
			break;
		case 'x':
			requiresFreeze = true;
			break;
		case '-':
			resetJumpOptions = true;
			break;
		case '@':
			selfChain = true;
			break;
		case 'a':
			if(counter) CHStats[ch].autoCorrects = 1;
			else stats[ch].autoCorrects = 1;
			break;
		case '/':
			if(counter) CHStats[ch].forceCrouch = 1;
			else stats[ch].forceCrouch = 1;
			break;
		case '\\':
			if(counter) CHStats[ch].forceStand = 1;
			else stats[ch].forceStand = 1;
			break;
		case '^':
			if(counter) CHStats[ch].launch = 1;
			else stats[ch].launch = 1;
			break;
		case '>':
			if(counter) CHStats[ch].wallBounce = 1;
			else stats[ch].wallBounce = 1;
			break;
		case 'v':
			if(counter) CHStats[ch].floorBounce = 1;
			else stats[ch].floorBounce = 1;
			break;
		case 'A':
			if(counter) CHStats[ch].ceilingBounce = 1;
			else stats[ch].ceilingBounce = 1;
			break;
		case '_':
			if(counter) CHStats[ch].slide = 1;
			else stats[ch].slide = 1;
			break;
		case '=':
			if(counter) CHStats[ch].stick = 1;
			else stats[ch].stick = 1;
			break;
		case 'u':
			if(!counter) unique = true;
			break;
		case 's':
			if(!counter) stop += 1;
			break;
		case 'S': 
			if(!counter) stop += 2;
			break;
		case '*':
			if(!counter) stop += 4;
			break;
		case 'c':
			if(!counter) crouch = 1;
			break;
		case ':':
			ch++;
			break;
		case 'H':
			if(!counter) hittable = 1;
			break;
		case 't':
			if(!counter) throwinvuln = 1;
			break;
		case 'T':
			if(!counter) throwinvuln = 2;
			break;
		case 'g':
			if(!counter) stats[ch].ghostHit = 1;
			break;
		case 'p':
			if(!counter) stats[ch].hitsProjectile = 1;
			break;
		case 'r':
			if(!counter) stats[ch].turnsProjectile = 1;
			break;
		case 'k':
			if(!counter) stats[ch].killsProjectile = 1;
			break;
		case 'd':
			if(!counter) dies = 1;
			break;
		case 'C':
			if(!counter) fch = 1;
			break;
		case 'h':
			if(!counter) hidesMeter = 1;
			break;
		case 'm':
			if(!counter) modifier = 1;
			break;
		case 'l':
			if(!counter) linkable = 1;
			break;
		case 'f':
			if(!counter) track = 1;
			break;
		case 'i':
			if(!counter) countersProjectile = false;
			break;
		case '~':
			freezeAgnostic = true;
			break;
		case '|':
			subsumedFreeze = true;
			break;
		default:
			break;
		}
	}
}

bool action::window(int f)
{
	if(modifier && basis.move){
		if(basis.move->window(basis.frame)) return 1;
	}
	if(!attempt) return 0;
	return attemptSpan == f;
}

bool action::activate(status &current, vector<int> inputs, int pattern, int t, int f)
{
	return patternMatch(inputs, pattern, t, f) && check(current) && *this > current;
}

bool action::patternMatch(vector<int> inputs, int pattern, int t, int f)
{
	for(unsigned int i = 0; i < inputs.size(); i++){
		if(pattern & (1 << i)){
			if(inputs[i] < minHold) return 0;
			if(maxHold && inputs[i] > maxHold) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return 1; 
}

bool action::check(const status &current)
{
	if(werf){
		if(current.opponent->save.throwInvuln > 0) return 0;
		if(werf == 1 && current.opponent->save.aerial) return 0;
		if(werf == 2 && !current.opponent->save.aerial) return 0;
	}
	if(restrictedMode)
		if(restrictedMode & current.mode)
			return 0;
	if(requiredMode)
		if(!(requiredMode & current.mode))
			return 0;
	if(unique){
		for(instance *i:current.offspring){
			if(i->pick() == payload) 
				return 0;
		}
	}
	if(cost && cost > current.meter[1].value)
		return 0;
	if(xRequisite > 0 && current.prox().w > xRequisite) 
		return 0;
	if(yRequisite > 0 && current.prox().h > yRequisite) 
		return 0;
	if(requiresFreeze && current.freeze <= 0)
		return 0;
	for(cooldownTracker i:current.cooldowns)
		if(i.move == this) return 0;
	return 1;
}

void action::pollRects(status &current, SDL_Rect &c, vector<SDL_Rect> &r, vector<SDL_Rect> &b)
{
	if(modifier && basis.move) basis.move->pollRects(basis, c, r, b);
	else {
		if(current.frame >= frames) current.frame = frames-1;
		if((unsigned int)current.frame < collision.size()) {
			c.x = collision[current.frame].x;
			c.y = collision[current.frame].y;
			c.w = collision[current.frame].w;
			c.h = collision[current.frame].h;
		}
		r.clear();
		if((unsigned int)current.frame < hitreg.size()) r = hitreg[current.frame];
		b.clear();
		if((unsigned int)current.frame < hitbox.size())
			if(current.connect <= calcCurrentHit(current.frame) || hits == 0) b = hitbox[current.frame];
	}
}

vector<SDL_Rect> action::pollDelta(int f)
{
	vector<SDL_Rect> ret;
	if(modifier && basis.move){
		ret = basis.move->pollDelta(basis.frame);
		for(SDL_Rect i:delta[f]) ret.push_back(i);
	} else if ((unsigned int)f < delta.size()) ret = delta[f];
	return ret;
}

bool action::displace(status &current)
{
	return current.frame == displaceFrame;
}

hStat action::pollStats(int f, bool CH)
{
	hStat s;
	if(modifier && basis.move) s = basis.move->pollStats(basis.frame, CH);
	else{
		int c = calcCurrentHit(f);
		s.damage = stats[c].damage + CHStats[c].damage * CH;
		s.chip = stats[c].chip;
		s.stun = stats[c].stun + CHStats[c].stun * CH;
		s.push = stats[c].push + CHStats[c].push * CH;
		s.lift = stats[c].lift + CHStats[c].lift * CH;
		s.initialLaunch = stats[c].initialLaunch + CHStats[c].initialLaunch * CH;
		s.untech = stats[c].untech + CHStats[c].untech * CH;
		s.blowback = stats[c].blowback + CHStats[c].blowback * CH;
		s.pause = stats[c].pause + CHStats[c].pause * CH;
		s.connect = stats[c].connect + CHStats[c].connect * CH;
		s.autoCorrects = stats[c].autoCorrects + CHStats[c].autoCorrects * CH;
		s.prorate = stats[c].prorate + (CHStats[c].prorate - 1.0) * CH;
		s.forceCrouch = stats[c].forceCrouch || CHStats[c].forceCrouch * CH;
		s.forceStand = stats[c].forceStand || CHStats[c].forceStand * CH;
		s.launch = stats[c].launch || CHStats[c].launch * CH;
		if(CH){
			s.hover = CHStats[c].hover;
			s.wallBounce = CHStats[c].wallBounce;
			s.floorBounce = CHStats[c].floorBounce;
			s.slide = CHStats[c].slide;
			s.stick = CHStats[c].stick;
			s.ghostHit = CHStats[c].ghostHit;
			s.ceilingBounce = CHStats[c].ceilingBounce;
		} else {
			s.hover = stats[c].hover;
			s.wallBounce = stats[c].wallBounce;
			s.floorBounce = stats[c].floorBounce;
			s.slide = stats[c].slide;
			s.stick = stats[c].stick;
			s.ghostHit = stats[c].ghostHit;
			s.ceilingBounce = stats[c].ceilingBounce;
		}
		s.hitsProjectile = stats[c].hitsProjectile;
		s.turnsProjectile = stats[c].turnsProjectile;
		s.killsProjectile = stats[c].killsProjectile;
		s.blockMask.i = stats[c].blockMask.i;
	}
	return s;
}

bool action::cancel(const status &current)
{
	cancelField r;
	action *z = current.move;
	r.i = 0;
	if(current.move == nullptr) return 1;
	if(current.hit < 0 || current.connect < 0 || current.connect > current.move->hits || current.hit > current.move->hits) return 0;
	if(current.move->modifier && current.move->basis.move){
		if(current.move->basis.move == nullptr){ 
			return 1;
		}
		r.i = current.move->basis.move->state[current.move->basis.connect].i;
		if(current.move->basis.hit > 0 && current.move->basis.hit == current.move->basis.connect){ 
			r.i = r.i + current.move->basis.move->stats[current.move->basis.hit - 1].hitState.i;
		}
		z = basis.move;
	} else {
		r.i = current.move->state[current.connect].i;
		if(current.hit > 0 && current.hit == current.connect){
			r.i = r.i + current.move->stats[current.hit - 1].hitState.i;
		}
	}
	if(allowed.i & r.i){
		if(z == this){
			if(current.connect == 0) return 0;
			else if(selfChain) return 1;
			else return 0;
		} else {
			return 1;
		}
	}
	return 0;
}

bool action::operator>(const status& o)
{
	return check(o) && cancel(o);
}

void action::step(status &current)
{
	if(current.connect == calcCurrentHit(current.frame)+1) 
		current.connect += stats[current.connect-1].connect;
	if(!current.frame && !current.meter[4].value){
		if(current.meter[1].value + gain[0] < 300) current.meter[1].value += gain[0];
		else current.meter[1].value = 300;
	}
	if(current.delay > 0){
		if(delayMax && current.delay >= delayMax){
			current.delay = 0;
			current.frame++;
		}
	} else current.frame++;
	if(modifier && basis.move){
		basis.frame++;
		if(basis.move && basis.frame >= basis.move->frames){
			if(basis.move->next)
				basis.move = basis.move->next->execute(basis);
			else {
				basis.move = nullptr;
				basis.frame = 0;
				basis.connect = 0;
				basis.hit = 0;
			}
		}
	}
}

int action::calcCurrentHit(int frame)
{
	int b = 0;
	for(int i = 0; i < hits; i++){
		if(frame > totalStartup[i]) b = i;
	}
	return b;
}

action * action::connect(status &current)
{
	basis.meter = current.meter;
	if(modifier && basis.move) return basis.move->connect(basis);
	else if (hits == 0) return nullptr;
	else {
		current.connect = calcCurrentHit(current.frame)+1;
		if(!current.meter[4].value){
			if(current.meter[1].value + gain[current.connect] < 300) current.meter[1].value += gain[current.connect];
			else current.meter[1].value = 300;
		}
		if(stats[current.connect-1].onConnect != nullptr){
			return stats[current.connect-1].onConnect;
		} else return nullptr;
	}
}

action * action::blockSuccess(int n, bool p)
{
	if(modifier && basis.move) return basis.move->blockSuccess(n, p);
	if(riposte){
		if(!p || countersProjectile) return riposte;
	}
	return this;
}

void action::playSound(int channel)
{
	Mix_PlayChannel(channel, soundClip, 0);
}

action * action::execute(status &current)
{
	current.absorbedHits = 0;
	current.meter[1].value -= cost;
	if(resetJumpOptions){
		current.meter[2].value = current.meter[2].maximum;
		current.meter[3].value = current.meter[3].maximum;
	}
	current.meter[4].value += cost * 2;
	if(modifier){
		if(current.move == nullptr) basis.move = nullptr;
		basis.move = current.move;
		basis.frame = current.frame;
		basis.connect = current.connect;
		basis.hit = current.hit;
	}
	if(cooldown) current.cooldowns.push_back({this, cooldown});
	if(freezeAgnostic) current.freeze = 0;
	current.frame = 0;
	current.connect = 0;
	current.hit = 0;
	return this;
}

void action::feed(action * c, int code, int i)
{
	switch(code){
	case 0:
		next = c;
		break;
	case 2:
		stats[i].onConnect = c;
		break;
	case 3:
		attempt = c;
		break;
	case 5:
		riposte = c;
		break;
	case 6:
		onHold = c;
		break;
	}
}

string action::request(int code, int i)
{
	switch(code){
	case 0:
		return tempNext;
	case 2:
		return tempOnConnect[i];
	case 3:
		return tempAttempt;
	case 4:
		return tempPayload;
	case 5:
		return tempRiposte;
	case 6:
		return tempOnHold;
	default:
		return "";
	}
}

bool action::operator!=(const string &o)
{
	return fileName.compare(o) ? true : false;
}

bool action::operator==(const string &o)
{
	return fileName == o;
}

bool action::canGuard(status &current)
{
	if(current.frame >= guardStart && current.frame <= guardStart + guardLength && blockState.i & 7) return true;
	else return false;
}

bool action::armor(status &current)
{
	return (current.frame >= armorStart && current.frame <= armorStart + armorLength && (armorHits < 1 || armorHits >= current.absorbedHits));
}

int action::takeHit(hStat & s, int blockType, status &current)
{
	if(modifier && basis.move) return basis.move->takeHit(s, blockType, current);
	else {
		if(!stunMin || s.stun >= stunMin){
			if(!stunMax || s.stun <= stunMax){
				if(s.blockMask.i & blockState.i && canGuard(current)){
					if(riposte != nullptr){
						if(!s.isProjectile || countersProjectile) return -5;
					}
					return guardType;
				}
			}
		}
		return 1;
	}
}

bool action::CHState(int f)
{
	if(fch) return true;
	if(modifier && basis.move) return basis.move->CHState(basis.frame);
	if(hits < 1) return false;
	if(f < totalStartup[hits-1] + active[hits-1]) return true;
	return false;
}

hStat::hStat(const hStat& o)
{
	this->damage = o.damage;
	this->chip = o.chip;
	this->stun = o.stun;
	this->pause = o.pause;
	this->push = o.push;
	this->lift = o.lift;
	this->untech = o.untech;
	this->blowback = o.blowback;
	this->hover = o.hover;
	this->launch = o.launch;
	this->ghostHit = o.ghostHit;
	this->wallBounce = o.wallBounce;
	this->floorBounce = o.floorBounce;
	this->ceilingBounce = o.ceilingBounce;
	this->slide = o.slide;
	this->stick = o.stick;
	this->hitsProjectile = o.hitsProjectile;
	this->turnsProjectile = o.turnsProjectile;
	this->killsProjectile = o.killsProjectile;
	this->isProjectile = o.isProjectile;
	this->connect = o.connect;
	this->prorate = o.prorate;
	this->blockMask.i = o.blockMask.i;
	this->hitState.i = o.hitState.i;
}
