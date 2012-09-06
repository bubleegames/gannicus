/*Interface class for GUFG
 *This will run all the main game functions within GUFG
 *
 *Written by Alex Kelly in 2012
 *License to come
 */

#include "interface.h"
#include <cstring>
#include <math.h>
#include <assert.h>
#include <SDL/SDL_opengl.h>
#include <algorithm>
#include <fstream>
#include <iostream>
interface::interface()
{
	char buffer[50];
	numChars = 3;
	std::ifstream read;
	/*Initialize some pseudo-constants*/
	screenWidth = 1600; //By screen, I mean the window the game occurs in.
	screenHeight = 900;
	screen = NULL;
	bg.w = 3200;       //By background, I mean the thing the characters actually move on. Bigger than the screen.
	bg.h = 1800;
	floor = bg.h - 50; //Value of the floor. This is the maximum distance downward that characters can travel.
	wall = 50;         //The size of the offset at which characters start to scroll the background, and get stuck.

	read.open("Misc/.res.conf");
	if(read.fail()){ 
		scalingFactor = 1.0;
		fullscreen = true;
	} else { 
		read >> scalingFactor;
		read.ignore(100, '\n');
		read >> fullscreen;
	}
	read.close();
	sf = scalingFactor;
	assert(screenInit() != false);

	/*Initialize players.*/
	for(int i = 0; i < 2; i++){
		p[i] = new player(i+1);
		sAxis[i] = new bool[4];
		posEdge[i] = new bool[5]; 
		negEdge[i] = new bool[5];
		counter[i] = 0;
		select[i] = 0;
		selection[i] = 1;
		sprintf(buffer, "Misc/P%iSelect%i.png", i+1, selection[i]);
		cursor[i] = aux::load_texture(buffer);
	}

	for(int i = 0; i < 5; i++){
		posEdge[0][i] = 0;
		posEdge[1][i] = 0;
		negEdge[0][i] = 0;
		negEdge[1][i] = 0;
		if(i < 4){
			sAxis[0][i] = 0;
			sAxis[1][i] = 0;
		}
	}

	/*Game and round end conditions*/
	gameover = 0;
	numRounds = 2;

	SDL_Event temp;
	while(SDL_PollEvent(&temp));

	/*Start a match*/
	things = NULL;
	selectScreen = aux::load_texture("Misc/Select.png");
	matchInit();
}

bool interface::screenInit()
{
	/*Initialize SDL*/
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) return false;
	/*WM stuff*/
	int h, w;
	if(scalingFactor == 1.0){ 
		w = screenWidth; h = screenHeight;
	} else {
		h = 450; w = 800;
	}
	if(screen){ 
		SDL_FreeSurface(screen);
		screen = NULL;
	}
	SDL_WM_SetCaption("GUFG", "GUFG");
	if(!fullscreen){
		if((screen = SDL_SetVideoMode(w, h, 32, SDL_OPENGL)) == NULL)
			return false;
	} else {
		if((screen = SDL_SetVideoMode(w, h, 32, SDL_OPENGL | SDL_FULLSCREEN)) == NULL)
			return false;
	}
	SDL_ShowCursor(SDL_DISABLE);

	/*Set up input buffers and joysticks*/
	for(int i = 0; i < SDL_NumJoysticks(); i++)
		SDL_JoystickOpen(i);
//	glDisable (GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable (GL_BLEND);
	glEnable (GL_POINT_SMOOTH);
	glEnable (GL_LINE_SMOOTH);
	glEnable (GL_POLYGON_SMOOTH);

	glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, h, 0, 1, -1);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	initd = true;
	return true;
}

/*This functions sets things up for a new match. Initializes some things and draws the background*/
void interface::matchInit()
{
	SDL_Event event;
	select[0] = 0;
	select[1] = 0;
	p[0]->rounds = 0;
	p[1]->rounds = 0;
	p[0]->secondInstance = 0;
	p[1]->secondInstance = 0;
	background = aux::load_texture("Misc/BG1.png");
	q = 0;
	matchIntro = 1;
	matchIntro = 0; //To be removed later, when match intro stuff actually exists
	printf("Please select a character:\n");
	while (SDL_PollEvent(&event));
}

/*Sets stuff up for a new round. This initializes the characters, the timer, and the background.*/
void interface::roundInit()
{
	if(things){ 
		delete [] things;
	}
	things = NULL;
	thingComplexity = 0;
	for(int i = 0; i < 2; i++)
		addThing(p[i]);
	thingComplexity = 2;
	bg.x = 800;
	bg.y = 900;

	for(int i = 0; i < 2; i++){
		p[i]->roundInit();
		p[i]->posY = floor - p[i]->cMove->collision[0].h;
	}
	/*Initialize input containers*/
	for(int i = 0; i < 4; i++) 
	{
		sAxis[0][i] = 0;
		sAxis[1][i] = 0;
	}
	for(int i = 0; i < 5; i++){
		posEdge[0][i] = 0;
		negEdge[0][i] = 0;
		posEdge[1][i] = 0;
		negEdge[1][i] = 0;
	}

	combo[0] = 0;
	combo[1] = 0;
	grav = 6;
	timer = 60 * 99;
	roundIntro = 1;
	roundIntro = 0;
	prox.w = 200;
	prox.h = 0;
	freeze = 0;
	draw();
}

/*Pretty simple timer modifier*/
void interface::runTimer()
{
	for(int i = 0; i < 2; i++){
		if(select[i] == true){
			if(p[i]->cMove != NULL)
			{
				timer += (p[i]->cMove->arbitraryPoll(31, p[i]->currentFrame));
				if(timer > 60*99) timer = 60*99;
			}
		}
	}
	if(timer > 0) timer--;
/*
	if(timer % 60 == 0) printf("%i seconds remaining\n", timer / 60);
	printf("%i frames remaining\n", timer);
//*/
}

/*Main function for a frame. This resolves character spritions, background scrolling, and hitboxes*/
void interface::resolve()
{
	if(!select[0] || !select[1]) cSelectMenu(); 
	else {
		for(int i = 0; i < thingComplexity; i++) things[i]->pushInput(sAxis[things[i]->ID - 1]);
		p[1]->getMove(posEdge[1], negEdge[1], prox, 1);
		for(int i = 0; i < thingComplexity; i++){
			if(i < 2){
				if(p[(i+1)%2]->pick()->aerial) prox.y = 1;
				else prox.y = 0;
				prox.x = p[(i+1)%2]->throwInvuln;
			}
			things[i]->getMove(posEdge[things[i]->ID - 1], negEdge[things[i]->ID - 1], prox, 0);
		}
	/*Current plan for this function: Once I've got everything reasonably functionally abstracted into player members,
	the idea is to do the procedure as follows:
		1. Update to current rectangles. Since the actual step is part of the draw routine, this should happen first.
		2. Figure out all deltas that should currently apply to sprite spritions. Basically move the sprites to where
		   they'd be if there were no collision rules.
		3. Check for things like hits and blocks. Enact all the effects of these, including stun, damage, etc.
		4. Check for collision against the opponent or against boundaries such as the floor or the corners.
		   Haven't decided if there should be a cieling yet.
		5. Apply any changes to deltas that need to be made before the next frame
		6. Initialize sprites.
	*/

		for(int i = 0; i < thingComplexity; i++)
			things[i]->updateRects();

		resolveThrows();
		doSuperFreeze();

		for(int i = 0; i < thingComplexity; i++)
			things[i]->updateRects();

		for(int i = 0; i < thingComplexity; i++){
			if(!things[i]->freeze){
				things[i]->pullVolition();
				things[i]->combineDelta();
				if(i < 2) p[i]->enforceGravity(grav, floor);
			}
		}

		dragBG(p[1]->dragBG(bg.x + wall, bg.x + screenWidth - wall) +
			p[0]->dragBG(bg.x + wall, bg.x + screenWidth - wall) );
		p[0]->checkCorners(floor, bg.x + wall, bg.x + screenWidth - wall);
		p[1]->checkCorners(floor, bg.x + wall, bg.x + screenWidth - wall);
		
		unitCollision();
		
		if(p[0]->cMove->state[p[0]->connectFlag].i & 1 && p[0]->cMove != p[0]->pick()->airNeutral) 
			p[0]->checkFacing(p[1]);
		if(p[1]->cMove->state[p[1]->connectFlag].i & 1 && p[1]->cMove != p[1]->pick()->airNeutral) 
			p[1]->checkFacing(p[0]);

		for(int i = 0; i < 2; i++){
			if(!p[i]->pick()->aerial) { p[i]->deltaX = 0; p[i]->deltaY = 0; }

			if(!p[i]->cMove->arbitraryPoll(1, 0)){
				combo[(i+1)%2] = 0;
				p[i]->elasticX = 0;
				p[i]->elasticY = 0;
			}
		}

		for(int i = 0; i < thingComplexity; i++){
			if(things[i]->hitbox[0].w > 0){
				if(!freeze) p[(things[i]->ID)%2]->checkBlocking();
			}
		}

		//Check if moves hit. This will probably be a function at some point
		resolveHits();

		/*Draw the sprites*/
		draw();
		for(int i = 0; i < thingComplexity; i++){
			things[i]->step();
			if(i > 1 && things[i]->dead) cullThing(i);
		}
		resolveSummons();
		if(!matchIntro && !roundIntro){
			checkWin();
			runTimer();
		}
	}
	/*Reinitialize inputs*/
	for(int i = 0; i < 5; i++){
		posEdge[0][i] = 0;
		posEdge[1][i] = 0;
		negEdge[0][i] = 0;
		negEdge[1][i] = 0;
	}

}

void interface::resolveSummons()
{
	action * temp;
	instance * larva;
	int x, y, f;
	for(int i = 0; i < thingComplexity; i++){
		if(things[i]->cMove){
			temp = things[i]->cMove;
			if(temp->arbitraryPoll(50, things[i]->currentFrame)){
				larva = things[i]->pick()->spawn(temp);
				larva->ID = things[i]->ID;
				if(temp->arbitraryPoll(51, things[i]->currentFrame)){
					x = p[(things[i]->ID)%2]->posX;
					f = p[(things[i]->ID)%2]->facing;
				} else {
					x = p[(things[i]->ID)-1]->posX;
					f = p[(things[i]->ID)-1]->facing;
				}
				if(temp->arbitraryPoll(52, things[i]->currentFrame))
					y = p[(things[i]->ID)%2]->posY;
				else
					y = p[(things[i]->ID)-1]->posY;
				x += temp->arbitraryPoll(53, things[i]->currentFrame)*f;
				y += temp->arbitraryPoll(54, things[i]->currentFrame);
				larva->facing = f;
				larva->setPosition(x, y);
				addThing(larva);
				larva->init();
			}
		}
	}
}

/*Check if someone won*/
void interface::checkWin()
{
	if(p[0]->pick()->health == 0 || p[1]->pick()->health == 0 || timer == 0){
		if(p[0]->pick()->health > p[1]->pick()->health) {
			printf("Player 1 wins!\n");
			p[0]->rounds++;
		}
		else if(p[1]->pick()->health > p[0]->pick()->health) {
			printf("Player 2 wins!\n");
			p[1]->rounds++;
		}
		else {
			printf("Draw!\n");
			if(p[0]->rounds < numRounds - 1) p[0]->rounds++;
			if(p[1]->rounds < numRounds - 1) p[1]->rounds++;
		}
		p[0]->momentumComplexity = 0;
		p[1]->momentumComplexity = 0;
		if(p[0]->rounds == numRounds || p[1]->rounds == numRounds){
			delete p[0]->pick();
			delete p[1]->pick();
			matchInit();
		}
		else roundInit();
	}
}

/*Read the input that's happened this frame*/
void interface::readInput()
{
	/*Make our dummy event for polls*/
	SDL_Event event;
	for(int i = 0; i < 20; i++){
		if (SDL_PollEvent(&event)){
			/*Do stuff with event*/
			for(int i = 0; i < 2; i++)
				p[i]->readEvent(event, sAxis[i], posEdge[i], negEdge[i]);
			switch (event.type){
			/*Kill handler*/
			case SDL_QUIT:
				gameover = 1;
				break;
				/*Keyboard handler. Maybe I'll optimize such that the knows if it even needs to check this (EG if sticks are used)*/
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_q:
				case SDLK_ESCAPE:
					gameover = 1;
					break;
				case SDLK_F10:
					if(scalingFactor == 1.0) sf = 0.5;
					else sf = 1.0;
					initd = false;
					break;
				case SDLK_F11:
					fullscreen = !fullscreen;
					initd = false;
					break;
				default:
					break;
				}
				break;
			}
		}
	}
}

void interface::cSelectMenu()
{
	/*The plan is that this is eventually a menu, preferably pretty visual, in which players can select characters.*/
	if(!initd){ 
		std::ofstream write;
		write.open("Misc/.res.conf");
		write << sf << '\n' << fullscreen;
		write.close();
		scalingFactor = sf;
		assert(screenInit() != false);
	}
	char base[2][40];

	for(int i = 0; i < 2; i++){
		if(sAxis[i][2] && !select[i] && counter[i] == 0){
			selection[i]--;
			if(selection[i] < 1) selection[i] = numChars;
			sprintf(base[i], "Misc/P%iSelect%i.png", i+1, selection[i]);
			cursor[i] = aux::load_texture(base[i]);
			counter[i] = 10;
		}
		if(sAxis[i][3] && !select[i] && counter[i] == 0){
			selection[i]++;
			if(selection[i] > numChars) selection[i] = 1;
			sprintf(base[i], "Misc/P%iSelect%i.png", i+1, selection[i]);
			cursor[i] = aux::load_texture(base[i]);
			counter[i] = 10;
		}
		for(int j = 0; j < 5; j++){
			if(posEdge[i][j] && !select[i]){
				select[i] = 1;
				p[i]->characterSelect(selection[i]);
			}
		}
	}
	
	glClear(GL_COLOR_BUFFER_BIT);

	glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
	glRectf(0.0f*scalingFactor, 0.0f*scalingFactor, (GLfloat)screenWidth*scalingFactor, (GLfloat)screenHeight*scalingFactor);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable( GL_TEXTURE_2D );
	glBindTexture(GL_TEXTURE_2D, selectScreen);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex3f(350.0f*scalingFactor, 0.0f*scalingFactor, 0.f*scalingFactor);

		glTexCoord2i(1, 0);
		glVertex3f(1250.0f*scalingFactor, 0.0f*scalingFactor, 0.f*scalingFactor);

		glTexCoord2i(1, 1);
		glVertex3f(1250.0f*scalingFactor, 900.0f*scalingFactor, 0.f*scalingFactor);

		glTexCoord2i(0, 1);
		glVertex3f(350.0f*scalingFactor, 900.0f*scalingFactor, 0.f*scalingFactor);
	glEnd();
	
	for(int i = 0; i < 2; i++){
		glBindTexture(GL_TEXTURE_2D, cursor[i]);
		glBegin(GL_QUADS);
			glTexCoord2i(0, 0);
			glVertex3f(350.0f*scalingFactor, 0.0f*scalingFactor, 0.f*scalingFactor);

			glTexCoord2i(1, 0);
			glVertex3f(1250.0f*scalingFactor, 0.0f*scalingFactor, 0.f*scalingFactor);

			glTexCoord2i(1, 1);
			glVertex3f(1250.0f*scalingFactor, 900.0f*scalingFactor, 0.f*scalingFactor);

			glTexCoord2i(0, 1);
			glVertex3f(350.0f*scalingFactor, 900.0f*scalingFactor, 0.f*scalingFactor);
		glEnd();
	}
	glDisable( GL_TEXTURE_2D );

	for(int i = 0; i < 2; i++) if(counter[i] > 0) counter[i]--;
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	SDL_GL_SwapBuffers();
	if(select[0] && select[1]){
		if(selection[0] == selection[1]) p[1]->secondInstance = true;
		roundInit();
	}
}

void interface::dragBG(int deltaX)
{
	bg.x += deltaX;
	if(bg.x < 0) bg.x = 0;
	else if(bg.x > 1600) bg.x = 1600;
}

interface::~interface()
{
	if(select[0]) delete p[0]->pick();
	if(select[1]) delete p[1]->pick();
	delete p[0];
	delete p[1];
	SDL_FreeSurface(screen);
	SDL_Quit();
}

void interface::unitCollision()
{
	player *right = p[1], *left = p[0];
	int middle[2];
	for(int i = 0; i < 2; i++){
		if(p[i]->facing == 1) middle[i] = p[i]->collision.x + p[i]->collision.w / 2;
		else middle[i] = p[i]->collision.x + p[i]->collision.w / 2 + p[i]->collision.w % 2;
	}
	if(middle[0] > middle[1]){ right = p[0]; left = p[1]; }
	else if(middle[0] < middle[1]){ right = p[1]; left = p[0]; }
	else {
		if(p[0]->facing == 1 && p[1]->facing == -1){ left = p[0]; right = p[1]; }
		else if(p[1]->facing == 1 && p[0]->facing == -1){ left = p[1]; right = p[0]; }
	}
	if (aux::checkCollision(p[0]->collision, p[1]->collision)){

	/*Collision between players. Unfortunately a lot of specialcasing necessary here.*/

		int rLOffset = right->posX - right->collision.x;
		int rROffset = right->posX - (right->collision.x + right->collision.w);
		int lLOffset = left->posX - left->collision.x;
		int lROffset = left->posX - (left->collision.x + left->collision.w);
		int dOffset = (left->deltaX - right->deltaX) % 2;
		int totalMiddle = (right->collision.x + left->collision.x + left->collision.w)/2;
		if(abs(left->deltaX) > abs(right->deltaX)) totalMiddle += dOffset;

		if(left->lCorner){ 
			right->posX = left->collision.x + left->collision.w + rLOffset;
		}
		else if(right->rCorner) left->posX = right->collision.x + lROffset;
		else {
			right->posX = totalMiddle + right->collision.w + rROffset;
			left->posX = totalMiddle - left->collision.w + lLOffset;
		}
		if(left->collision.x < 50) {
//			left->checkCorners(floor, bg.x + wall, bg.x + screenWidth - wall);
			left->updateRects();
			right->posX = left->collision.x + left->collision.w + rLOffset;
		} else if (right->collision.x + right->collision.w > 3150) {
//			right->checkCorners(floor, bg.x + wall, bg.x + screenWidth - wall);
			right->updateRects();
			left->posX = right->collision.x + lROffset;
		}
		right->updateRects();
		left->updateRects();
	}
	prox.w = right->posX - left->posX;
	prox.h = abs(right->posY - left->posY);
}

void interface::resolveThrows()
{
	bool isThrown[2] = {false, false};
	for(int i = 0; i < 2; i++){
		if(p[i]->cMove->arbitraryPoll(28, p[i]->currentFrame)) isThrown[(i+1)%2] = true;
	}
	if(isThrown[0] && isThrown[1]){
		p[0]->cMove = p[0]->pick()->throwBreak;
		p[1]->cMove = p[1]->pick()->throwBreak;
	} else {
		for(int i = 0; i < 2; i++){
			if(isThrown[i]){
				p[i]->getThrown(p[(i+1)%2]->cMove, p[(i+1)%2]->posX*p[(i+1)%2]->facing, p[(i+1)%2]->posY);
				p[i]->checkFacing(p[(i+1)%2]);
			}
		}
	}
}

void interface::resolveHits()
{
	hStat s[thingComplexity];
	int hit[thingComplexity];
	bool connect[thingComplexity];
	bool taken[thingComplexity];
	int hitBy[thingComplexity];
	for(int i = 0; i < thingComplexity; i++){
		taken[i] = 0;
		hit[i] = 0;
		connect[i] = 0;
		hitBy[i] = -1;
	}
	SDL_Rect residual = {0, 0, 1, 0};
	for(int i = 0; i < thingComplexity; i++){
		for(int h = 0; h < thingComplexity; h++){
			if(h != i && !taken[h]){
				for(int j = 0; j < things[i]->hitComplexity; j++){
					for(int k = 0; k < things[h]->regComplexity; k++){
						if(aux::checkCollision(things[i]->hitbox[j], things[h]->hitreg[k])){
							if(things[i]->acceptTarget(things[h])){
								connect[i] = 1;
								things[i]->cMove->pollStats(s[i], things[i]->currentFrame, things[h]->CHState());
								k = things[h]->regComplexity;
								j = things[i]->hitComplexity;
								taken[h] = 1;
								hitBy[h] = i;
							}
						}
					}
				}
			}
		}
	}

	for(int i = 0; i < thingComplexity; i++){
		if(connect[i]){
			things[i]->connect(combo[things[i]->ID-1], s[i]);
			if(i < 2 && p[i]->cMove->allowed.i < 128 && !p[i]->pick()->aerial) p[i]->checkFacing(p[(i+1)%2]);
		}
	}

	for(int i = 0; i < 2; i++){ 
		if(taken[i]){
			hit[hitBy[i]] = p[i]->takeHit(combo[hitBy[i]], s[hitBy[i]]);
			combo[(i+1)%2] += hit[hitBy[i]];
			if(hit[hitBy[i]] == 1) things[hitBy[i]]->hitFlag = things[hitBy[i]]->connectFlag;
			p[(i+1)%2]->checkCorners(floor, bg.x + wall, bg.x + screenWidth - wall);
			if(p[i]->facing * p[(i+1)%2]->facing == 1) p[i]->invertVectors(1);
		for(int i = 0; i < 2; i++)
			if(combo[i] > 1) printf("Player %i: %i hit combo\n", i+1, combo[i]);
		}
	}


	for(int i = 0; i < 2; i++){ 
		if(connect[i]){
			if(p[i]->pick()->aerial) residual.y = -8;
			else{ 
				if(p[(i+1)%2]->pick()->aerial) residual.x = -2;
				else {
					if(combo[i] > 1) residual.x = -2*(abs(combo[i]-1));
					if(p[(i+1)%2]->rCorner || p[(i+1)%2]->lCorner){
						residual.x -= 2;
						residual.x -= s[i].push/2;
						residual.x -= abs(combo[i]);
					}
				}
				p[i]->addVector(residual);
			}
		}
	}

	if(connect[0] || connect[1]){
		unitCollision();
	}

	for(int i = 0; i < 2; i++) {
		p[i]->throwInvuln--;
		p[i]->hover--;
	}
}

void interface::doSuperFreeze()
{
	int go[2] = {0, 0};
	for(int i = 0; i < 2; i++){
		go[i] = p[i]->cMove->arbitraryPoll(2, p[i]->currentFrame);
		if(go[i] > 0){ 
			p[(i+1)%2]->checkBlocking();
			p[(i+1)%2]->freeze += go[i];
		}
	}
	if(go[0] > 0 || go[1] > 0)
		freeze = std::max(go[0], go[1]);
}

void interface::addThing(instance *v)
{
	int i;
	instance ** temp;
	temp = new instance*[thingComplexity+1];
	for(i = 0; i < thingComplexity; i++)
		temp[i] = things[i];
	temp[i] = v;
	if(thingComplexity > 0) delete [] things;
	things = temp;
	thingComplexity++;
}

void interface::cullThing(int q)
{
	for(int i = q; i < thingComplexity - 1; i++)
		things[i] = things[i+1];
	thingComplexity--;
}
