/*Copyright Somnambulant Studios 2012*/
#include "interface.h"
#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <math.h>

using std::cout;
using std::ifstream;
using std::ofstream;
using std::max;
using std::min;
using std::to_string;

void resolveRematchMenu(menu*);
void resolvePauseMenu(menu*);
SaltAndBone::SaltAndBone() : pauseMenu(this), rematchMenu(this)
{
	title = "Fighting Game";
	stats = nullptr;
	initCharacters();
	displayMode = 0;
	killTimer = false;
	shortcut = true;
	continuous = false;
	analytics = true;
	scripting = false;
	pauseEnabled = false;
	single = false;
	ifstream read;
	globalAnnounce.x = 0;
	globalAnnounce.y = 5.0 / 12.0;
	globalAnnounce.w = 1.0;
	globalAnnounce.h = 1.0 / 6.0;
	globalAnnounce.R = 1.0;
	globalAnnounce.G = 1.0;
	globalAnnounce.B = 1.0;
	globalAnnounce.A = 1.0;
	globalAnnounce.align = 1;

	/*Initialize some pseudo-constants*/
	screen = nullptr; /*The screen gives a partial view of the env.background, which is the area available for character movement.*/
	menuMusic = nullptr;

	read.open(".config/settings.conf");
	if(read.fail()){ 
		scalingFactor = 0.5;
		musicVolume = 100;
	} else {
		read >> scalingFactor >> displayMode;
		read.ignore(100, '\n');
		read >> musicVolume;
	}
	read.close();
	sf = scalingFactor;
	w = env.screenWidth*sf; h = env.screenHeight*sf;

	/*Game and round end conditions*/
	gameover = 0;
	numRounds = 2;

	initContainers(2, 6);
	oldReplay = nullptr;
	replayIterator = 0;
	replay = nullptr;

	pauseMenu.runResolver = resolvePauseMenu;
	pauseMenu.labels.push_back("Unpause");
	pauseMenu.labels.push_back("Select Characters");
	pauseMenu.labels.push_back("Quit Game");
	
	rematchMenu.runResolver = resolveRematchMenu;
	rematchMenu.labels.push_back("Rematch");
	rematchMenu.labels.push_back("Select Characters");
	rematchMenu.labels.push_back("Quit Game");
}

void SaltAndBone::handleArgs(vector<string> args)
{
	for(string i:args) {
		if(i == "eleven"){
			killTimer = true;
			pauseEnabled = true;
		}
	}
}

void SaltAndBone::initShaders()
{
	//string version(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
	//version.erase(1,1);
	//Currently we've only dealt with two shaders, which we have code for. 
	//Ostensibly we need to look for a closest version or a threshold version where syntax changed.
	prog = shaderProgram("", "src/shaders/pink130.frag");
	window::initShaders();
}

void SaltAndBone::createPlayers(string rep)
{
	oldReplay = new script(rep);
	createPlayers();
	if(oldReplay->selection.size() != 2){
		printf("Invalid Replay\n");
		delete oldReplay;
		oldReplay = nullptr;
	} else {
		single = true;
		analytics = false;
		for(int i = 0; i < 2; i++){
			selection[i] = oldReplay->selection[i];
			select[i] = 1;
			P[i]->characterSelect(generateCharacter(selection[i]));
			if(scripting) P[i]->readScripts();
		}
		loadMatchBackground();
	}
}

void SaltAndBone::createPlayers()
{
	/*Initialize players.*/
	for(int i = 0; i < 2; i++){
		vector<int> a;
		P.push_back(new player(i+1));
		p.push_back(P[i]);
		select.push_back(false);
		selection.push_back(1+i);
		combo.push_back(0);
		knockdown.push_back(0);
		illegitHits.push_back(a);
		damage.push_back(0);
		blockFail.push_back(0);
		initialHealth.push_back(0);
		counterHit.push_back(0);
		punish.push_back(0);
		illegit.push_back(false);
		mMenu[i] = 0;
		configMenu[i] = 0;
		things.push_back(P[i]);
		P[i]->boxen = true;
		P[i]->sprite = false;
	}
	for(int i = 0; i < 2; i++){
		P[i]->current.opponent = P[(i+1)%2];
	}
}

void SaltAndBone::loadMatchBackground()
{
	switch (selection[0]){
	case 1:
		env.bgR = 1.0;
		env.bgG = 0.0;
		env.bgB = 0.0;
		break;
	case 2:
		env.bgR = 1.0;
		env.bgG = 1.0;
		env.bgB = 0.0;
		break;
	case 3:
		env.bgR = 0.25;
		env.bgG = 0.25;
		env.bgB = 0.25;
		break;
	case 4:
		env.bgR = 1.0;
		env.bgG = 0.5;
		env.bgB = 0.0;
		break;
	}

	if(selection[0] == selection[1]) 
		matchMusic = Mix_LoadMUS("content/sound/Mirror.ogg");
	else
		matchMusic = Mix_LoadMUS(("content/sound/" + to_string(selection[1]) + ".ogg").c_str());
}

void SaltAndBone::startGame()
{
	SDL_Event temp;
	while(SDL_PollEvent(&temp));

	/*Start a match*/
	//Mix_PlayChannel(3, announceSelect, 0);
	matchInit();
	if(select[0] && select[1]){ 
		roundInit();
	}
}

/*This function loads a few miscellaneous things the game will need in all cases*/
void HUD::loadMisc()
{
	for(int i = 0; i < 91; i++){
		glyph.push_back(aux::load_texture("content/glyphs/"+to_string(i)+".png"));
	}
}

void SaltAndBone::loadMisc()
{
	env.loadMisc();
	HUD::loadMisc();
	selectScreen = aux::load_texture("content/menu/Select.png");
	menuMusic = Mix_LoadMUS("content/sound/Menu.ogg");
	announceWinner = new Mix_Chunk*[numChars + 1];
	for(int i = 0; i < numChars + 1; i++){
		announceWinner[i] = Mix_LoadWAV(("content/sound/announcer/Win"+to_string(i)+".ogg").c_str());
	}
	for(unsigned int i = 0; i < p.size(); i++){
		if(!p[i]->readConfig(i+1)) initialConfig(i);
	}
	stats->init();
	announceRound[0] = Mix_LoadWAV("content/sound/announcer/Round1.ogg");
	announceRound[1] = Mix_LoadWAV("content/sound/announcer/Round2.ogg");
	announceRound[2] = Mix_LoadWAV("content/sound/announcer/RoundF.ogg");
	announceDraw[1] = Mix_LoadWAV("content/sound/announcer/Draw.ogg");
	announceFight = Mix_LoadWAV("content/sound/announcer/Fight.ogg");
	announceEnd[0] = Mix_LoadWAV("content/sound/announcer/Timeout.ogg");
	announceEnd[1] = Mix_LoadWAV("content/sound/announcer/Down.ogg");
	announceSelect = Mix_LoadWAV("content/sound/announcer/Select.ogg");
}

/*Initialize SDL and openGL, creating a window, among other things*/
bool fightingGame::screenInit()
{
	bool ret = window::screenInit();
	glDisable(GL_DEPTH_TEST);
	glDisable (GL_LIGHTING);
	Mix_OpenAudio(44100, AUDIO_S16, 2, 2048);
	SDL_ShowCursor(SDL_DISABLE);

	/*Set up input buffers and joycurrent.sticks*/
	init();
	initd = ret;
	return ret;
}

bool fightingGame::screenInit(int width, int height)
{
	w = width; h = height;
	return screenInit();
}

void fightingGame::init()
{
	//soundScape::init();
	harness::init();
}

void fightingGame::initialConfig(int ID)
{
	glPushMatrix();
		glScalef(scalingFactor, scalingFactor, 1.0f);
		p[ID]->input.clear();
		for(unsigned int i = 0; i < p[ID]->inputName.size(); i++){
			glClear(GL_COLOR_BUFFER_BIT);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glRectf(0.0f, 0.0f, (GLfloat)env.screenWidth, (GLfloat)env.screenHeight);
			glEnable( GL_TEXTURE_2D );
			glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
			drawGlyph("Player " + to_string(ID + 1), 0, env.screenWidth, 300, 80, 1);
			drawGlyph("Please enter a", 0, env.screenWidth, 400, 80, 1);
			drawGlyph("command for " + p[ID]->inputName[i], 0, env.screenWidth, 500, 80, 1);
			SDL_GL_SwapBuffers();
			glDisable( GL_TEXTURE_2D );
			glClear(GL_COLOR_BUFFER_BIT);
			p[ID]->setKey(1 << i);
		}
		p[ID]->writeConfig(ID+1);
	glPopMatrix();
}

	/*This functions sets things up for a new match. Initializes some things and draws the env.background*/
void SaltAndBone::matchInit()
{
	SDL_Event event;
	rematchMenu.on = 0;
	for(player* i:P){
		i->rounds = 0;
	}
	pauseMenu.on = 0;
	if(!select[0] || !select[1]){
		Mix_VolumeMusic(musicVolume);
		Mix_PlayMusic(menuMusic, -1);
		//printf("\n");
	}
	while (SDL_PollEvent(&event));
}

/*Sets stuff up for a new round. This initializes the characters, the timer, and the env.background.*/
void SaltAndBone::roundInit()
{
	roundEnd = false;
	while(things.size() > P.size())
		things.pop_back();
	env.roundInit();
	env.spawn(things);
	for(player* i:P){
		i->roundInit();
		i->updateRects();
	}
	/*Initialize input containers*/
	initContainers();
	for(unsigned int i = 0; i < P.size(); i++){
		combo[i] = 0;
		knockdown[i] = 0;
		prorate[i] = 1.0;
		damage[i] = 0;
		illegit[i] = 0;
		counterHit[i] = 0;
		punish[i] = 0;
		blockFail[i] = 0;
	}

	timer = 60 * 101;
	endTimer = 60 * 5;
	freeze = 0;
}

/*Pretty simple timer modifier*/
void SaltAndBone::runTimer()
{
	if(freeze > 0) freeze--;
	if(P[0]->rounds == 0 && P[1]->rounds == 0 && timer == 101 * 60){
		Mix_VolumeMusic(musicVolume);
		Mix_PlayMusic(matchMusic,-1);
	}
	int plus;
	for(unsigned int i = 0; i < P.size(); i++){
		if(select[i] == true){
			if(things[i]->current.move != nullptr){
				plus = (things[i]->current.move->arbitraryPoll(31, things[i]->current.frame));
				if(plus != 0){
					timer += plus;
					if(timer > 60*99) timer = 60*99 + 1;
				}
			}
		}
	}

	if(roundEnd){ 
		if(endTimer <= 60 * 3) endTimer = 0; 
		/*This is a temporary measure since we don't have winposes yet.
		 *This will eventually be replaced by the *ability* to skip them,
		 *But currently does what would happen if you skipped them every time.
		 */
		if(endTimer > 0) endTimer--;
		else{
			things[0]->current.momentum.clear();
			things[1]->current.momentum.clear();
			if(P[0]->rounds == numRounds || P[1]->rounds == numRounds){
				if(!oldReplay){
					if(P[0]->rounds == P[1]->rounds);
					else if(P[0]->rounds == numRounds){ 
						P[0]->wins++;
						//printf("P1: %i wins\n", P[0]->wins);
					} else {
						P[1]->wins++;
						//printf("P2: %i wins\n", P[1]->wins);
					}
					if(P[0]->rounds == P[1]->rounds);
					else{
						if(P[0]->rounds == numRounds) stats->recordWin(selection[0], selection[1]);
						else stats->recordWin(selection[1], selection[0]);
					}
					//printf("Matchup: %f\n", stats->matchup(selection[0], selection[1]));
				}
				if(shortcut) rematchMenu.on = 1;
				else{
					if(!continuous){
						for(unsigned int i = 0; i < P.size(); i++){
							delete P[i]->pick();
							select[i] = 0;
						}
					}
					if(SDL_WasInit(SDL_INIT_VIDEO) != 0){
						Mix_HaltMusic();
						Mix_FreeMusic(matchMusic);
					}
					if(analytics && replay){
						replay->write();
						delete replay;
						replay = nullptr;
					}
					for(player *i:P){
						if(i->record){
							i->record->write(to_string(i->ID) + "-" + i->pick()->name + ".sh");
							delete i->record;
							i->record = nullptr;
						}
					}
					if(oldReplay){
						delete oldReplay;
						oldReplay= nullptr;
					}
					if(single) gameover = true;
					else{
						matchInit();
						if(select[0] && select[1]){
							if(analytics){
								replay = new script;
								replay->init(selection);
							}
							roundInit();
						}
					}
				}
			}
			else roundInit();
		}
	} else if(!killTimer || timer > 99 * 60) timer--;
}

void fightingGame::print()
{
	cout << "\x1b[A" << "\x1b[A";
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 80; j++) cout << " ";
		cout << ('\n');
	}
	for(player* i:P) i->print();
}

/*Main function for a frame. This resolves character spritions, env.background scrolling, and current.hitboxes*/
void SaltAndBone::resolve()
{
	harness::processInput();
	if(!select[0] || !select[1]) cSelectMenu();
	else if(rematchMenu) rematchMenu();
	else if(pauseMenu) pauseMenu();
	else {
		resolveInputs();
		for(instance *i:things) i->updateRects();
		resolveThrows();
		doSuperFreeze();
		for(instance *i:things) i->updateRects();
		resolvePhysics();
		resolveCamera();
		resolveCollision();
		resolveCombos();
		resolveHits();
	}
}

void SaltAndBone::initCharacters()
{
	ifstream nch;
	numChars = 0;
	char buffer[200];
	string temp;
	characterManifest.push_back("White");
	nch.open("src/charlist.h");
	do{
		nch.getline(buffer, 200);
		if(buffer[0] == '/'){
			tokenizer t(buffer, "/\n -");
			charTable[stoi(t())] = nullptr;
			characterManifest.push_back(t());
			numChars++;
		}
	} while(!nch.eof());
	nch.close();
	if(stats) delete stats;
	stats = new chart(numChars);
}

void SaltAndBone::resolveCombos()
{
	for(unsigned int i = 0; i < P.size(); i++){
		if(!roundEnd){
			switch (P[i]->comboState()){
			case -3:
				knockdown[(i+1)%2] = -1;
				break;
			case 2:
				knockdown[(i+1)%2] = 1;
				break;
			case -2:
				illegit[(i+1)%2] = 1;
				counterHit[(i+1)%2] = 0;
				punish[(i+1)%2] = 0;
				blockFail[i] = 0;
				break;
			case 0:
				if(killTimer && !freeze){ 
					P[i]->current.meter[0].value = 600;
					if(combo[i] == 0 && P[i]->current.opponent->current.cancelState() & 1){
						P[i]->current.opponent->current.meter[1].value = 300;
						P[i]->current.opponent->current.meter[4].value = 0;
					}
				}
				knockdown[(i+1)%2] = 0;
				combo[(i+1)%2] = 0;
				damage[(i+1)%2] = 0;
				initialHealth[i] = P[i]->current.meter[0].value;
				prorate[(i+1)%2] = 1.0;
				P[i]->current.elasticX = 0;
				P[i]->current.elasticY = 0;
				P[i]->current.rebound = 0;
				illegit[(i+1)%2] = 0;
				illegitHits[(i+1)%2].clear();
				counterHit[(i+1)%2] = 0;
				punish[(i+1)%2] = 0;
				blockFail[i] = 0;
				break;
			}
		}
	}
}

void SaltAndBone::resolveCamera()
{
	/*Really rudimentary camera logic. Really just scrolls the env.background (Which characters are drawn relative to)
	 *appropriately, attempting to adjust to approximately be looking at the point in the middle of the two characters.
	 */
	int dx = things[1]->dragBG(env.bg.x + env.wall, env.bg.x + env.screenWidth - env.wall) + things[0]->dragBG(env.bg.x + env.wall, env.bg.x + env.screenWidth - env.wall);
	int dy = env.screenHeight;

	/*If a character leaves the camera boundaries, follow them immediately*/
	if(!dx){
		dx = -(((env.bg.x + env.screenWidth/2) - things[0]->current.posX) + ((env.bg.x + env.screenWidth/2) - things[1]->current.posX));
		dx /= 10;
		/*Otherwise follow the middle at a rate of (disparity from middle view)/10.
		 *Chosen by trial and error, this rate feels most natural
		 */
	}
	dragBG(dx);
	for(unsigned int i = 0; i < P.size(); i++){
		if(dy < things[i]->current.posY + things[i]->current.collision.h){
			dy = things[i]->current.posY + things[i]->current.collision.h;
			if(dy > env.bg.h) dy = env.bg.h;
		}
	}
	for(instance *i:things) env.enforceBounds(i);
	env.bg.y = dy - env.bg.h;
}

void SaltAndBone::resolveInputs()
{
	if(timer > 99 * 60){
		for(unsigned int i = 0; i < P.size(); i++){
			if(timer == 106 * 60) things[i]->inputBuffer[0] = 0;
			else if(timer == 106 * 60 - 1) things[i]->inputBuffer[0] = i;
			else if(timer == 106 * 60 - 2) things[i]->inputBuffer[0] = selection[(i+1)%2] / 10;
			else if(timer == 106 * 60 - 3) things[i]->inputBuffer[0] = selection[(i+1)%2] % 10;
			else if(timer == 106 * 60 - 4) things[i]->inputBuffer[0] = 0;
			else(things[i]->inputBuffer[0] = 5);
			for(int &j:currentFrame[i].buttons) j = 0;
		}
	} else {
		int flop[2] = {0, 0};
		for(unsigned int i = 0; i < currentFrame.size(); i++){
			if(P[i]->current.facing == -1){
				if(currentFrame[i].n.raw.dir % 3 == 0) flop[i] -= 2;
				else if(currentFrame[i].n.raw.dir % 3 == 1) flop[i] += 2; 
			}
		}
		for(player *i:P){
			i->pushInput(currentFrame[i->ID - 1].n.raw.dir + flop[i->ID -1]);
			i->directionalInfluence();
			i->getMove(currentFrame[i->ID - 1].buttons);
		}
	}
	for(unsigned int i = 0; i < P.size(); i++){
		if(analytics)
			replay->push(i, currentFrame[i].n);
		if(P[i]->record)
			P[i]->record->push(currentFrame[i].n);
	}
}

void SaltAndBone::resolvePhysics()
{
	for(instance *i:things){
		if(!i->current.freeze){
			if(i->ID) i->follow(things[i->ID%2]);
			if(!(i->current.move->stop & 4)){
				i->pullVolition();
				i->combineDelta();
				env.airCheck(i);
				env.enforce(i);
			}
		}
	}
}

void resolveRematchMenu(menu* m)
{
	switch(m->cursor){
	case 0:
		Mix_HaltMusic();
		m->toggle();
		m->game->matchInit();
		break;
	case 1:
		for(unsigned int k = 0; k < m->game->P.size(); k++){
			m->game->P[k]->characterSelect(nullptr);
			m->game->select[k] = 0;
			m->game->things[k]->current.meter.clear();
		}
		Mix_HaltMusic();
		Mix_FreeMusic(m->game->matchMusic);
		//Mix_PlayChannel(3, announceSelect, 0);
		m->game->matchInit();
		break;
	case 2:
		Mix_HaltMusic();
		Mix_FreeMusic(m->game->matchMusic);
		m->game->gameover = 1;
		break;
	}
}

void resolvePauseMenu(menu* m)
{
	switch(m->cursor){
	case 0:
		m->toggle();
		break;
	case 1:
		for(unsigned int i = 0; i < m->game->P.size(); i++){
			m->game->P[i]->characterSelect(nullptr);
			m->game->select[i] = 0;
			m->game->things[i]->current.meter.clear();
		}
		Mix_HaltMusic();
		Mix_FreeMusic(m->game->matchMusic);
		//Mix_PlayChannel(3, announceSelect, 0);
		m->game->matchInit();
		break;
	case 2:
		Mix_HaltMusic();
		Mix_FreeMusic(m->game->matchMusic);
		m->game->gameover = 1;
		break;
	}
}

void SaltAndBone::cleanup()
{
	if(select[0] && select[1] && !pauseMenu){
		for(instance *i:things){
			i->cleanup();
			if(i->current.posX > env.bg.w + 300 || i->current.posX < -300 || i->current.posY < -300 || i->current.posY > env.bg.h){
				i->outOfBounds();
			}
		}
		if(!rematchMenu && select[0] && select[1]){
			for(instance *i:things) i->step();
			for(unsigned int i = 0; i < things.size(); i++){
				if(i > 1 && things[i]->current.dead){ 
					things.erase(things.begin()+i);
					i--;
				}
			}
			env.cleanup();
			resolveSummons();
			summonForces();
			if(!roundEnd) checkWin();
			runTimer();
		}
	}

	for(unsigned int i = 0; i < currentFrame.size(); i++){
		if(currentFrame[i].n.raw.Start == 1 && counter[i] <= 0){
			if(pauseEnabled && !roundEnd){
				pauseMenu.toggle();
				counter[i] = 10;
			}
		}
	}

	for(unsigned int i = 0; i < currentFrame.size(); i++){
		for(unsigned int j = 0; j < currentFrame[i].buttons.size(); j++){
			if(currentFrame[i].buttons[j] != 0) currentFrame[i].buttons[j]++;
		}
		currentFrame[i].n.raw.Start = 0;
		currentFrame[i].buttons[5] = 0;
		if(counter[i] > 0) counter[i]--;
	}

	harness::cleanup();
}

void SaltAndBone::resolveSummons()
{
	instance * larva;
	int x, y, f;
	for(unsigned int i = 0; i < things.size(); i++){
		if(things[i]->current.move){
			if(things[i]->current.move->arbitraryPoll(50, things[i]->current.frame)){
				larva = things[i]->spawn();
				if(larva){
					larva->boxen = things[i]->boxen;
					larva->sprite = things[i]->sprite;
					switch (things[i]->current.move->arbitraryPoll(56, things[i]->current.frame)){
					case 0:
						larva->ID = 0;
						larva->current.opponent = P[(things[i]->ID)%2];
						break;
					case 1:
						larva->ID = things[i]->ID;
						larva->current.opponent = P[(things[i]->ID)%2];
						break;
					case 2:
						larva->ID = (things[i]->ID)%2+1;
						larva->current.opponent = P[(things[i]->ID)-1];
						break;
					}
					if(things[i]->current.move->arbitraryPoll(51, things[i]->current.frame)){
						x = things[(things[i]->ID)%2]->current.posX;
						f = things[(things[i]->ID)%2]->current.facing;
					} else {
						x = things[(things[i]->ID)-1]->current.posX;
						f = things[(things[i]->ID)-1]->current.facing;
					}
					if(things[i]->current.move->arbitraryPoll(52, things[i]->current.frame))
						y = things[(things[i]->ID)%2]->current.posY;
					else if(things[i]->current.move->arbitraryPoll(53, things[i]->current.frame))
						y = 0;
					else
						y = things[(things[i]->ID)-1]->current.posY;
					x += things[i]->current.move->arbitraryPoll(54, things[i]->current.frame)*f;
					y += things[i]->current.move->arbitraryPoll(55, things[i]->current.frame);
					if(x > env.bg.w + 100) x = env.bg.w + 100;
					if(y > env.bg.h - 50) y = env.bg.h - 50;
					if(x < -100) x = -100;
					larva->current.facing = f;
					larva->setPosition(x, y);
					things.push_back(larva);
					larva->init();
				}
			}
		}
	}
}

void SaltAndBone::summonForces()
{
	shared_ptr<force> tvec{nullptr}, avec{nullptr};
	for(unsigned int i = 0; i < things.size(); i++){
		if(things[i]->current.move && things[i]->current.frame == things[i]->current.move->distortSpawn) tvec = things[i]->current.move->distortion;
		if(tvec != nullptr){

			auto avec = makeForce(tvec->type);
			avec->x = tvec->x;
			avec->y = tvec->y;
			avec->type = tvec->type;
			avec->length = tvec->length;
			if(avec->length > 0){
				avec->origin = nullptr;
				avec->check = nullptr;
			} else {
				avec->origin = things[i];
				avec->check = things[i]->current.move;
			}
			avec->radius = tvec->radius;
			avec->effectCode = tvec->effectCode;
			avec->eventHorizon = tvec->eventHorizon;
			avec->grip = tvec->grip;
			if(things[i]->current.facing == 1) avec->current.posX = things[i]->current.collision.x + things[i]->current.collision.w / 2;
			else avec->current.posX = things[i]->current.collision.x - things[i]->current.collision.w / 2 + things[i]->current.collision.w % 2;
			avec->current.posY = things[i]->current.collision.y + things[i]->current.collision.h/2;
			if(avec->type == 0) avec->x *= things[i]->current.facing;
			switch(tvec->ID){
			case 1:
				avec->ID = things[i]->ID;
				break;
			case 2:
				avec->ID = (things[i]->ID % 2) + 1;
				break;
			default:
				avec->ID = 0;
				break;
			}
			env.physics.push_back(avec);
			avec = nullptr;
			tvec = nullptr;
		}
	}
}

/*Check if someone won*/
void SaltAndBone::checkWin()
{
	if(P[0]->isDead() || P[1]->isDead() || timer == 0){
		roundEnd = true;
		if(P[0]->current.meter[0].value > P[1]->current.meter[0].value) {
			P[0]->rounds++;
		}
		else if(P[1]->current.meter[0].value > P[0]->current.meter[0].value) {
			P[1]->rounds++;
		}
		else {
			if(P[0]->rounds < numRounds - 1 || continuous) P[0]->rounds++;
			if(P[1]->rounds < numRounds - 1 || continuous) P[1]->rounds++;
		}
	}
}

void fightingGame::genInput()
{
	if(oldReplay){
		for(unsigned int i = 0; i < p.size(); i++)
			oldReplay->genEvent(i, replayIterator, currentFrame[i].n);
		replayIterator++;
		if(replayIterator > oldReplay->command[0].size()){
			delete oldReplay;
			oldReplay = nullptr;
			replayIterator = 0;
		}
	} else {
		if(!pauseEnabled){
			for(unsigned int i = 0; i < P.size(); i++){
				if(P[i]->currentMacro){
					if(!P[i]->currentMacro->genEvent(0, P[i]->iterator, currentFrame[i].n)){
						P[i]->currentMacro = nullptr;
					} else {
						if(P[i]->current.facing == -1){
							bool s = currentFrame[i].axis[2];
							currentFrame[i].axis[2] = currentFrame[i].axis[3];
							currentFrame[i].axis[3] = s;
						}
						P[i]->iterator++;
					}
				}
			}
		}
	}
}

/*Read the input that's happened this frame*/
void SaltAndBone::processInput(SDL_Event &event)
{
	/*Do stuff with event*/
	for(unsigned int i = 0; i < p.size(); i++){
		for(unsigned int j = 0; j < p.size(); j++)
			if(!P[j]->currentMacro) P[j]->readEvent(event, currentFrame[j]);
		int t = p[i]->tap(event);
		if(t == 0) t = p[(i+1)%2]->tap(event);
		if((t < 1 || t > 8) && (t < 512) && event.type != SDL_JOYHATMOTION){
			if(p[i]->same(event)){
				if(configMenu[i] > 1 && configMenu[i] < 7){
					p[i]->swapKey(1 << (configMenu[i]+2), event);
					configMenu[i]++;
					counter[i] = 10;
				}
			}
		}
	}
	fightingGame::processInput(event);
}

void SaltAndBone::readInput()
{
	harness::readInput();
	if(select[0] && select[1]){
		if(scripting){
			for(player* i:P){
				for(SDL_Event j:events){
					if(!oldReplay){
						bool r = false;
						if(i->record) r = true;
						i->macroCheck(j);
						if(i->record){
							i->search = false;
							if(!r) break;
						}
					}
				}
				if(i->search){
					for(unsigned int j = 0; j < events.size(); j++){
						if((i->currentMacro = i->patternMatch(abs(i->tap(events[j]))))){
							i->search = false;
							i->iterator = 0;
							break;
						}
					}
				}
			}
		}
		for(frame i:currentFrame){
			for(auto j:i.axis) j = 0;
			i.n.i = 0;
		}
	}
	if(scripting || oldReplay) genInput();
}

void fightingGame::processInput(SDL_Event &event)
{
	switch (event.type){
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_F10:
			sf = (scalingFactor == 1.0) ? 0.5 : 1.0;
			initd = false;
			break;
		case SDLK_F11:
			initd = false;
			break;
		default:
			harness::processInput(event);
			break;
		}
		break;
	default:
		harness::processInput(event);
		break;
	}
}

void SaltAndBone::cSelectMenu()
{
	/*The plan is that this is eventually a menu, preferably pretty visual, in which players can select characters.*/
	if(!initd){ 
		ofstream write;
		write.open(".config/settings.conf");
		write << sf << ' ' << displayMode << '\n';
		write << musicVolume << '\n';
		write.close();
		scalingFactor = sf;
		assert(screenInit(env.screenWidth*sf, env.screenHeight*sf) != false);
	}

	for(unsigned int i = 0; i < P.size(); i++){
		if(numChars < 2){
			select[i] = 1;
			selection[i] = 1;
		} else {
			if(!mMenu[i]){
				if(currentFrame[i].axis[2] && !select[i] && counter[i] == 0){
					selection[i]--;
					if(selection[i] < 1) selection[i] = numChars;
					counter[i] = 10;
				}
				if(currentFrame[i].axis[3] && !select[i] && counter[i] == 0){
					selection[i]++;
					if(selection[i] > numChars) selection[i] = 1;
					counter[i] = 10;
				}
				for(int j = 0; j < 5; j++){
					if(currentFrame[i].buttons[j] == 1 && !select[i]){
						select[i] = 1;
						P[i]->selectedPalette = j;
					}
				}
				if(currentFrame[i].n.raw.Start){
					if(!select[i]) mMenu[i] = 3;
					else {
						select[i] = 0;
					}
					counter[i] = 10;
				}
			}
		}
	}

	for(unsigned int i = 0; i < P.size(); i++){
		if(configMenu[i] > 0) keyConfig(i);
		else if(mMenu[i] > 0) mainMenu(i);
	}

	if(select[0] && select[1]){
		//cout << "2 6\n" << selection[0] << " " << selection[1] << '\n';
		if(selection[0] == selection[1] && P[0]->selectedPalette == P[1]->selectedPalette){ 
			P[1]->selectedPalette = (P[1]->selectedPalette + 1) % 5;
		}
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_TEXTURE_2D);
		drawLoadingScreen();
		SDL_GL_SwapBuffers();
		for(unsigned int i = 0; i < P.size(); i++) {
			P[i]->characterSelect(generateCharacter(selection[i]));
			P[i]->current.meter = P[i]->pick()->generateMeter();
			P[i]->neutralize();
			P[i]->iterator = 0;
		}
		loadAssets();
		if(analytics){
			replay = new script;
			replay->init(selection);
		}

		Mix_HaltMusic();

		roundInit();
	}
}

character * fightingGame::generateCharacter(int i)
{
	if(charTable[i] == nullptr)
		charTable[i] = new character(characterManifest[i]);
	return charTable[i];
}

character * SaltAndBone::generateCharacter(int i)
{
	if(charTable[i] == nullptr){
		switch(i){
		case 2:
			charTable[i] = new yellow;
			break;
		default:
			return fightingGame::generateCharacter(i);
			break;
		}
	}
	return charTable[i];
}

void SaltAndBone::loadAssets()
{
	unsigned int b = SDL_WasInit(SDL_INIT_VIDEO);
	for(int i = 0; i < 2; i++){
		string temp = "content/sound/";
		temp += P[i]->pick()->name + ".wav";
		//call[i] = sample(temp);
		//alSourcei(source[i], AL_BUFFER, call[i]());
	}
	if(b != 0){
		for(player *i:P) i->loadAssets();
		loadMatchBackground();
	}
}

void SaltAndBone::mainMenu(int ID)
{
	if(currentFrame[ID].axis[0] && !counter[ID]){
		mMenu[ID]--;
		counter[ID] = 10;
	} else if(currentFrame[ID].axis[1] && !counter[ID]){
		mMenu[ID]++;
		counter[ID] = 10;
	}
	if(mMenu[ID] > 9) mMenu[ID] = 1;
	else if(mMenu[ID] < 1) mMenu[ID] = 9;
	for(unsigned int i = 0; i < currentFrame[ID].buttons.size()-1; i++){
		if(currentFrame[ID].buttons[i] == 1 && !counter[ID]){
			switch(mMenu[ID]){
			case 1:
				analytics = !analytics;
				break;
			case 2:
				configMenu[ID] = 7;
				break;
			case 3:
				mMenu[ID] = 0;
				break;
			case 4:
				shortcut = !shortcut;
				break;
			case 5:
				if(scripting)
					scripting = false;
				else{
					pauseEnabled = false;
					scripting = true;
				}
				break;
			case 6:
				if(pauseEnabled)
					pauseEnabled = false;
				else {
					scripting = false;
					pauseEnabled = true;
				}
				break;
			case 7:
				if(P[ID]->boxen && P[ID]->sprite) P[ID]->sprite = false;
				else if(P[ID]->boxen){
					P[ID]->sprite = true;
					P[ID]->boxen = false;
				} else P[ID]->boxen = true;
				break;
			case 8:
				killTimer = !killTimer;
				break;
			case 9:
				gameover = 1;
				break;
			}
			counter[ID] = 10;
		}
	}
	if(currentFrame[ID].n.raw.Start && !counter[ID]){ 
		counter[ID] = 10;
		mMenu[ID] = 0;
	}
}

void SaltAndBone::keyConfig(int ID)
{
	if(currentFrame[ID].axis[0] && !counter[ID]){
		configMenu[ID]--;
		counter[ID] = 10;
	} else if(currentFrame[ID].axis[1] && !counter[ID]){
		configMenu[ID]++;
		counter[ID] = 10;
	}
	if(configMenu[ID] > 7) configMenu[ID] = 1;
	else if(configMenu[ID] < 1) configMenu[ID] = 7;
	for(unsigned int i = 0; i < currentFrame[ID].buttons.size()-1; i++){
		if(currentFrame[ID].buttons[i] == 1 && !counter[ID]){
			switch(configMenu[ID]){
			case 1:
				glDisable( GL_TEXTURE_2D );
				initialConfig(ID);
				glEnable( GL_TEXTURE_2D );
				configMenu[ID] = 7;
				break;
			case 7:
				configMenu[ID] = 0;
				mMenu[ID] = 2;
				p[ID]->writeConfig(ID+1);
				break;
			}
			counter[ID] = 10;
		}
	}
	if(currentFrame[ID].n.raw.Start == 1 && !counter[ID]){ 
		counter[ID] = 10;
		configMenu[ID] = 0;
		mMenu[ID] = 0;
		p[ID]->writeConfig(ID+1);
	}
}

void SaltAndBone::dragBG(int dx)
{
	env.bg.x += dx;
	if(env.bg.x < 0) env.bg.x = 0;
	else if(env.bg.x > env.bg.w - env.screenWidth) env.bg.x = env.bg.w - env.screenWidth;
}

SaltAndBone::~SaltAndBone()
{
	for(unsigned int i = 0; i < characterManifest.size(); i++){
		if(charTable[i] != nullptr){ 
			delete charTable[i];
		}
	}
	charTable.clear();
	if(menuMusic != nullptr) Mix_FreeMusic(menuMusic);
	delete stats;
	SDL_FreeSurface(screen);
	SDL_Quit();
}

void fightingGame::unitCollision(instance *a, instance *b)
{
	instance *right, *left;
	if(a->middle() > b->middle()){
		right = a; left = b;
	} else if(a->middle() < b->middle()){
		right = b; left = a;
	} else {
		if((a->current.facing == 1 && b->current.facing == -1) || a->current.lCorner || b->current.rCorner){
			left = a; right = b;
		} else if((b->current.facing == 1 && a->current.facing == -1) || a->current.rCorner || b->current.lCorner){
			left = b; right = a;
		} else {
			left = a; right = b;
		}
	}

	/*Collision between players. Unfortunately a lot of specialcasing necessary here.*/

	int rLOffset = right->current.posX - right->current.collision.x,
	    rROffset = right->current.posX - (right->current.collision.x + right->current.collision.w),
	    lLOffset = left->current.posX - left->current.collision.x,
	    lROffset = left->current.posX - (left->current.collision.x + left->current.collision.w),
	    dOffset = (left->current.deltaX - right->current.deltaX) % 2,
	    totalMiddle = (right->current.collision.x + left->current.collision.x + left->current.collision.w)/2;
	if(abs(left->current.deltaX) > abs(right->current.deltaX)) totalMiddle += dOffset;

	if(left->current.lCorner){ 
		right->current.posX = left->current.collision.x + left->current.collision.w + rLOffset;
	}
	else if(right->current.rCorner) left->current.posX = right->current.collision.x + lROffset;
	else {
		right->current.posX = totalMiddle + right->current.collision.w + rROffset;
		left->current.posX = totalMiddle - left->current.collision.w + lLOffset;
	}
	if(left->current.collision.x < 50) {
		left->updateRects();
		right->current.posX = left->current.collision.x + left->current.collision.w + rLOffset;
	} else if (right->current.collision.x + right->current.collision.w > 3150) {
		right->updateRects();
		left->current.posX = right->current.collision.x + lROffset;
	}
	right->updateRects();
	left->updateRects();
}

void SaltAndBone::resolveCollision()
{
	vector<SDL_Rect> temp;
	vector<int> dx;
	for(instance *i:things){
		env.enforceFloor(i);
		temp.push_back(i->current.collision);
		dx.push_back(i->current.deltaX);
		temp.back().x -= dx.back();
	}

	unsigned int localMaximum = 20;
	unsigned int j[2] = {0, 0};
	while((int)j[0] < abs(dx[0]) || (int)j[1] < abs(dx[1])){
		if(aux::checkCollision(temp[0], temp[1])){
			unsigned int k[2] = {j[0], j[1]};
			while(dx[0] || dx[1]){
				for(unsigned int i = 0; i < P.size(); i++){
					if(dx[i]){
						P[i]->current.posX += temp[i].x - P[i]->current.collision.x;
						P[i]->updateRects();
						dx[i] -= (dx[i] > 0) ? j[i] : -j[i];
						j[i] = 0;
					}
				}
				for(player *i:P) env.checkCorners(i);
				unitCollision(P[0], P[1]);
				for(unsigned int i = 0; i < P.size(); i++){
					if(localMaximum < abs(dx[i]) - j[i]){
						j[i] += localMaximum;
						temp[i].x += (dx[i] > 0) ? localMaximum : -localMaximum;
					} else {
						j[i] = abs(dx[i]);
						temp[i].x += (dx[i] > 0) ? (j[i] - k[i]) : -(j[i] - k[i]);
					}
				}
			}
		} else {
			for(unsigned int i = 0; i < P.size(); i++){
				if((int)j[i] < abs(dx[i])) {
					if(dx[i] < 0){
						temp[i].x--;
					} else if(dx[i] > 0){
						temp[i].x++;
					}
					j[i]++;
				}
			}
		}
	}

	for(unsigned int i = 0; i < things.size(); i++){
		if(things[i]->current.move->track){
			things[i]->checkFacing(P[(things[i]->ID)%2]);
		}
		if(i < 2 && !things[i]->current.aerial) {
			things[i]->current.deltaX = 0; 
			things[i]->current.deltaY = 0; 
		}
	}

	for(instance *i:things){
		env.enforceFloor(i);
		env.enforceBounds(i);
		env.checkCorners(i);
	}

	//Some issues arise if you don't have this second pass
	if (aux::checkCollision(P[0]->current.collision, P[1]->current.collision))
		unitCollision(P[0], P[1]);
}

void SaltAndBone::resolveThrows()
{
	bool isThrown[2] = {false, false};
	for(unsigned int i = 0; i < P.size(); i++){
		if(things[i]->current.move->arbitraryPoll(28, things[i]->current.frame)){ 
			isThrown[(i+1)%2] = true;
		}
	}
	if(isThrown[0] || isThrown[1]){
		for(player *i:P) i->checkFacing();
	}
	if(isThrown[0] && isThrown[1]){
		for(player *i:P) i->current.move = i->pick()->throwBreak;
	} else {
		for(unsigned int i = 0; i < P.size(); i++){
			if(isThrown[i]){
				P[i]->checkFacing();
				P[i]->updateRects();
				P[i]->getThrown(P[i]->current.opponent->current.move, P[i]->current.opponent->current.posX*P[i]->current.opponent->current.facing, P[i]->current.opponent->current.posY);
				P[i]->checkFacing();
			}
		}
	}
}

void SaltAndBone::comboScaling(hStat &d, int ID)
{
	bool actuallyDoesDamage = (d.damage > 0);
	d.untech -= combo[ID];
	d.damage *= damage[ID] > 0 ? 1.0 - (float)damage[ID] / initialHealth[(ID + 1) % 2] : 1.0;
	d.damage *= prorate[ID];
	if(actuallyDoesDamage){
		d.damage -= combo[ID];
		if(d.damage < 1) d.damage = 1;
	}
}

void SaltAndBone::resolveHits()
{
	vector<hStat> s(things.size());
	vector<int> hit(things.size());
	vector<bool> connect(things.size());
	vector<bool> taken(things.size());
	vector<int> hitBy(things.size());
	int push[2];
	for(player *i:P) env.checkCorners(i);
	for(unsigned int i = 0; i < things.size(); i++){
		taken[i] = 0;
		hit[i] = 0;
		connect[i] = 0;
		hitBy[i] = -1;
	}
	for(instance *i:things){
		if(!i->current.hitbox.empty()){
			if(!freeze) i->current.opponent->checkBlocking();
		}
	}

	vector<int> chID;
	for(instance *i:things) chID.push_back(i->ID);

	for(unsigned int i = 0; i < things.size(); i++){
		for(int j = things.size()-1; j >= 0; j--){
			if(j != (int)i){
				if(!taken[j] && !connect[i] && things[i]->acceptTarget(things[j])){
					if(things[i]->checkHit(things[j])){
						connect[i] = 1;
						taken[j] = 1;
						hitBy[j] = i;
						things[i]->current.counter = things[j]->CHState();
						s[i] = things[i]->pollStats();
						comboScaling(s[i], things[i]->ID-1);
						if(things[i]->current.counter > 0){
							counterHit[things[i]->ID-1] = 1;
						} else if(!(things[j]->current.cancelState() & 513) && !things[j]->current.counter && !combo[things[i]->ID-1]){
							punish[things[i]->ID-1] = 1;
						}
						if(i < P.size()) push[i] = s[i].push;
						break;
					}
				}
			}
		}
	}

	for(unsigned int i = 0; i < things.size(); i++){ 
		if(taken[i]){
			if(i < 2){
				if(P[i]->comboState() == -2){
					illegitHits[(i+1)%2].push_back(combo[(i+1)%2]);
				}
			}
			int health = things[things[i]->ID-1]->current.meter[0].value;
			action * b = things[i]->current.move;
			bool wasair = things[i]->current.aerial;
			if(things[i]->current.move->reflectorSpan == things[i]->current.frame && things[hitBy[i]]->pick()->turn(things[hitBy[i]]->ID))
				things[hitBy[i]]->turnX();
			else hit[hitBy[i]] = things[i]->takeHit(s[hitBy[i]]);
			if(i < P.size()){
				if(hit[hitBy[i]] == 1){
					if(b->canGuard(P[i]->current)){
						blockFail[i] = s[hitBy[i]].blockMask.i;
						if(!blockFail[i]) blockFail[i] = 8;
						else if(wasair && !(blockFail[i] & 4)) blockFail[i] = 12;
					}
					if(combo[(i+1)%2] < 0) combo[(i+1)%2] = 0;
				}
				if(things[i]->current.particleType == -2){
					hStat parryHit = P[i]->generateParry(s[hitBy[i]]);
					damage[i] += parryHit.damage;
					P[things[hitBy[i]]->ID-1]->takeHit(parryHit);
				}
				if(s[hitBy[i]].stun){ 
					
					combo[(i+1)%2] += hit[hitBy[i]];
				}
			}
			env.enforceFloor(things[i]->current.opponent);
			env.enforceBounds(things[i]->current.opponent);
			env.checkCorners(things[i]->current.opponent);
			if(things[i]->current.facing * things[hitBy[i]]->current.facing == 1) things[i]->invertVectors(1);
			if(i < P.size()){ 
				damage[(i+1)%2] += health - P[i]->current.meter[0].value;
			}
		}
	}

	for(unsigned int i = 0; i < things.size(); i++){
		if(connect[i] && chID[i] == things[i]->ID){
			things[i]->connect(s[i]);
			if(hit[i] == 1){
				things[i]->current.hit = things[i]->current.connect;
				prorate[things[i]->ID-1] *= s[i].prorate;
			}
		}
	}

	bool bounce;
	SDL_Rect residual = {0, 0, 1, 0};
	for(unsigned int i = 0; i < P.size(); i++){ 
		bounce = false;
		if(connect[i]){
			if(P[i]->current.aerial){ 
				switch(s[i].pause){
				case -1:
					bounce = true;
					residual.y = 2 + (s[i].stun/4+9)/4;
					break;
				case 0:
					residual.y = 0;
					break;
				default:
					bounce = true;
					residual.y = 2 + (s[i].pause - 1)/4;
					break;
				}
			} else { 
				if(P[i]->current.opponent->current.aerial){
					if(P[i]->current.opponent->current.rCorner || P[i]->current.opponent->current.lCorner) residual.x -= abs(combo[i]);
					if(P[i]->current.opponent->current.stick) residual.x -= s[i].push/2 + abs(combo[i]);
					residual.x -= 2;
				} else {
					if(combo[i] > 1) residual.x = -3*(abs(combo[i]-1));
					if(P[i]->current.opponent->current.particleType == -2) residual.x -= push[i];
					else if(P[i]->current.opponent->current.rCorner || P[i]->current.opponent->current.lCorner){
						residual.x -= 2;
						residual.x -= s[i].push/2;
						residual.x -= abs(combo[i]);
					}
				}
				residual.x *= things[i]->current.facing;
			}
			if(!s[i].ghostHit) things[i]->current.momentum.push_back(residual);
			if(bounce) things[i]->current.momentum.push_back({0, 6, 0, 0});
		}
	}

	for(instance * i:P) {
		if(i->current.meter[0].value <= 0 && endTimer >= 5 * 60){ 
			for(instance *j:things)
				j->current.freeze = 30;
			break;
		}
	}
}

void SaltAndBone::doSuperFreeze()
{
	int go[2] = {0, 0};
	for(unsigned int i = 0; i < P.size(); i++){
		if(!things[i]->current.move->subsumedFreeze || freeze <= 0){
			go[i] = things[i]->current.move->arbitraryPoll(2, things[i]->current.frame);
		}
	}

	for(unsigned int i = 0; i < P.size(); i++){
		if(go[i] > 0){
			P[i]->current.opponent->checkBlocking();
			P[i]->current.opponent->current.freeze += go[i] - go[(i+1)%2];
			if(things[i]->current.move->arbitraryPoll(32, 0)){
				for(unsigned int j = 2; j < things.size(); j++) things[j]->current.freeze += go[i];
			}
		}
	}

	if(go[0] > 0 || go[1] > 0){
		freeze = max(go[0], go[1]);
	}
}

