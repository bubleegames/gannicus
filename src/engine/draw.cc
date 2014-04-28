/*Copyright Somnambulant Studios 2012-2013*/
#include "auxil.h"
#include "gl-compat.h"
#include "interface.h"
#include "session.h"
#include "thing.h"
#include <SDL/SDL_opengl.h>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include "window.h"
#include "shader.h"
#include "menu.h"

using std::to_string;

void SaltAndBone::draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
		glScalef(scalingFactor, scalingFactor, 0.0f);
		glViewport(0, 0, env.screenWidth*scalingFactor, env.screenHeight*scalingFactor);
		if(P.size() > 1 && (!select[0] || !select.back())) drawCSelect();
		else drawGame();
		if(rematchMenu) rematchMenu.draw();
		else if(pauseMenu) pauseMenu.draw();
	glPopMatrix();
	if(screenShot){
		glReadPixels(0, 0, w, h, GL_RGBA, ___gufg_tex_mode, image->pixels);
	}

	SDL_GL_SwapBuffers();
}

void SaltAndBone::drawCSelect()
{
	int x, y;
	glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
	glRectf(0.0f, 0.0f, (GLfloat)env.screenWidth, (GLfloat)env.screenHeight);

	for(int i = 0; i < 2; i++){
		if(configMenu[i]) drawConfigMenu(i);
		else if (mMenu[i]) drawMainMenu(i);
	}
	glEnable( GL_TEXTURE_2D );

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, selectScreen);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex3f(350.0f, 0.0f, 0.f);

		glTexCoord2i(1, 0);
		glVertex3f(1250.0f, 0.0f, 0.f);

		glTexCoord2i(1, 1);
		glVertex3f(1250.0f, 900.0f, 0.f);

		glTexCoord2i(0, 1);
		glVertex3f(350.0f, 900.0f, 0.f);
	glEnd();

	for(int i = 0; i < 2; i++){
		if(!mMenu[i]){
			x = ((float)env.screenWidth/2.0 + ((float)env.screenHeight/3.0) * cos(((M_PI*2.0)/(float)numChars)*(float)selection[i]+M_PI/4.0+M_PI/2.0)) - 100.0;
			y = ((float)env.screenHeight/2.0 + ((float)env.screenHeight/3.0) * sin(((M_PI*2.0)/(float)numChars)*(float)selection[i]+M_PI/4.0+M_PI/2.0));
			glColor4f(0.0, 0.3+i*0.3, 0.3+(1-i)*0.3, 1.0-select[i]*0.5);
			drawGlyph("P" + to_string(i+1), x, 200, y, 50, i*2);
		}
	}

	glDisable( GL_TEXTURE_2D );

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void SaltAndBone::drawMainMenu(int ID)
{
	glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
	glRectf(0.0f + 800.0 * ID, 0.0, (env.screenWidth/2*ID) + (GLfloat)env.screenWidth/2.0, (GLfloat)env.screenHeight);
	glEnable( GL_TEXTURE_2D );
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(mMenu[ID] == 1)*0.4);
	if(analytics)
		drawGlyph("Replay", 20 + 1260*ID, 300, 270, 40, 2*ID);
	else
		drawGlyph("No Replay", 20 + 1260*ID, 300, 270, 40, 2*ID);

	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(mMenu[ID] == 2)*0.4);
	drawGlyph("Key Config", 20 + 1260*ID, 300, 310, 40, 2*ID);
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(mMenu[ID] == 3)*0.4);
	drawGlyph("Exit Menu", 20 + 1260*ID, 300, 350, 40, 2*ID);

	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(mMenu[ID] == 4)*0.4);
	if(shortcut)
		drawGlyph("Rematch", 20 + 1260*ID, 300, 390, 40, 2*ID);
	else
		drawGlyph("Reselect", 20 + 1260*ID, 300, 390, 40, 2*ID);

	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(mMenu[ID] == 5)*0.4);
	if(scripting)
		drawGlyph("Scripts On", 20 + 1260*ID, 300, 430, 40, 2*ID);
	else
		drawGlyph("Scripts Off", 20 + 1260*ID, 300, 430, 40, 2*ID);

	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(mMenu[ID] == 6)*0.4);
	if(pauseEnabled) 
		drawGlyph("Pause on", 20 + 1260*ID, 300, 470, 40, 2*ID);
	else 
		drawGlyph("Pause off", 20 + 1260*ID, 300, 470, 40, 2*ID);

	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(mMenu[ID] == 7)*0.4);
	if(P[ID]->boxen && P[ID]->sprite) 
		drawGlyph("Both", 20 + 1260*ID, 300, 510, 40, 2*ID);
	else if(P[ID]->boxen) 
		drawGlyph("Boxen", 20 + 1260*ID, 300, 510, 40, 2*ID);
	else
		drawGlyph("Sprites", 20 + 1260*ID, 300, 510, 40, 2*ID);

	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(mMenu[ID] == 8)*0.4);
	if(killTimer)
		drawGlyph("Training", 20 + 1260*ID, 300, 550, 40, 2*ID);
	else
		drawGlyph("Match", 20 + 1260*ID, 300, 550, 40, 2*ID);
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(mMenu[ID] == 9)*0.4);
	drawGlyph("Quit Game", 20 + 1260*ID, 300, 590, 40, 2*ID);
	glDisable( GL_TEXTURE_2D );
	glColor4f(1.0, 1.0, 1.0, 1.0f);
}

void SaltAndBone::drawConfigMenu(int ID)
{
	int i;
	glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
	glRectf(0.0f + 800.0 * ID, 0.0, (env.screenWidth/2*ID) + (GLfloat)env.screenWidth/2.0, (GLfloat)env.screenHeight);
	glEnable( GL_TEXTURE_2D );
	glColor4f(1.0, 1.0, 0.0, 0.4 + (float)(configMenu[ID] == 1)*0.4);
	switch(p[ID]->input[0]->trigger.type){
	case SDL_KEYDOWN:
		drawGlyph("Keyboard", 20 + 1260*ID, 300, 310, 40, 2*ID);
		break;
	case SDL_JOYBUTTONDOWN:
		drawGlyph("Joy " + to_string(p[ID]->input[0]->trigger.jbutton.which), 20 + 1260*ID, 300, 310, 40, 2*ID);
		break;
	case SDL_JOYAXISMOTION:
		drawGlyph("Joy " + to_string(p[ID]->input[0]->trigger.jaxis.which), 20 + 1260*ID, 300, 310, 40, 2*ID);
		break;
	case SDL_JOYHATMOTION:
		drawGlyph("Joy " + to_string(p[ID]->input[0]->trigger.jhat.which), 20 + 1260*ID, 300, 310, 40, 2*ID);
		break;
	}
	for(i = 2; i < 7; i++){
		glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(configMenu[ID] == i)*0.4);
		drawGlyph(p[ID]->inputName[i+2], 20 + 1230*ID, 300, 310+40*(i-1), 40, 0);
		for(unsigned int j = 0; j < p[ID]->input.size(); j++)
			if(p[ID]->input[j]->effect == 1 << (i+2)){
				glColor4f(1.0, 1.0, 0.0, 0.4 + (float)(configMenu[ID] == i)*0.4);
				switch(p[ID]->input[j]->trigger.type){
				case SDL_KEYDOWN:
					drawGlyph(to_string(p[ID]->input[j]->trigger.key.keysym.sym),
							  70 + 1230*ID, 300, 310+40*(i-1), 40, 0);
					break;
				case SDL_JOYBUTTONDOWN:
					drawGlyph("B" + to_string(p[ID]->input[j]->trigger.jbutton.button),
							  70 + 1230*ID, 300, 310+40*(i-1), 40, 0);
					break;
				case SDL_JOYAXISMOTION:
					drawGlyph("Axis "+ to_string(p[ID]->input[j]->trigger.jaxis.axis) 
							  + " " + to_string(p[ID]->input[j]->trigger.jaxis.value), 
							  70 + 1230*ID, 300, 310+40*(i-1), 40, 0);
					break;
				case SDL_JOYHATMOTION:
					drawGlyph("Hat "+ to_string(p[ID]->input[j]->trigger.jhat.hat) 
							  + " " + to_string(p[ID]->input[j]->trigger.jhat.value), 
							  70 + 1230*ID, 300, 310+40*(i-1), 40, 0);
					break;
				default:
					glColor4f(1.0, 0.0, 0.0, 0.4 + (float)(configMenu[ID] == i)*0.4);
					drawGlyph("Not Set", 70 + 1230*ID, 300, 310+40*(i-1), 40, 0);
					break;
			}
		}
	}
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(configMenu[ID] == 7)*0.4);
	drawGlyph("Exit Menu", 20 + 1260*ID, 300, 310+40*(i-1), 40, 2*ID);
	glDisable( GL_TEXTURE_2D );
	glColor4f(1.0, 1.0, 1.0, 1.0f);
}

void SaltAndBone::drawLoadingScreen()
{
	glPushMatrix();
		glScalef(scalingFactor, scalingFactor, 0.0f);
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glRectf(0.0f, 0.0f, (GLfloat)env.screenWidth, (GLfloat)env.screenHeight);
		glEnable(GL_TEXTURE_2D);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		drawGlyph("Loading", 0, env.screenWidth, (env.screenHeight - 64)/2, 64, 1);
	glPopMatrix();
}

void environment::draw()
{
	glPushMatrix();
		glTranslatef(-bg.x, bg.y, 0);
		glColor4f(bgR, bgG, bgB, 0.5f);
		glRectf(0, 0, bg.w, bg.h);
		glEnable( GL_TEXTURE_2D );
		if(background){
			glColor4f(1.0, 1.0, 1.0, 0.125);
//			glColor4f(1.0 - bgR, 1.0 - bgG, 1.0 - bgB, 0.5);
			glBindTexture(GL_TEXTURE_2D, background);
			glBegin(GL_QUADS);
				glTexCoord2i(0, 0);
				glVertex3f(0.0f, 0.0f, 0.f);

				glTexCoord2i(1, 0);
				glVertex3f((float)bg.w, 0.0f, 0.f);

				glTexCoord2i(1, 1);
				glVertex3f((float)bg.w, (float)bg.h, 0.f);

				glTexCoord2i(0, 1);
				glVertex3f(0.0, (float)bg.h, 0.f);
			glEnd();
		}
	glPopMatrix();
	glColor4f(1.0, 1.0, 1.0, 1.0);
}

void SaltAndBone::drawGame()
{
	if(!demo){
		env.draw();
		drawHUD();
	} else {
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glRectf(0, 0, env.bg.w, env.bg.h);
	}
	glPushMatrix();
		glTranslatef(-env.bg.x, (env.bg.y+env.bg.h), 0);
		if(demo){
			P[0]->draw(prog());
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		} else {
			for(instance *i:things){ 
				i->draw(prog());
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
		for(player *i:P) i->drawHitParticle();
		glEnable( GL_TEXTURE_2D );
		glDisable( GL_TEXTURE_2D );
	glPopMatrix();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glEnable( GL_TEXTURE_2D );
	glDisable( GL_TEXTURE_2D );

	if(freeze > 0){
		glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
		glRectf(0.0f, 0.0f, (GLfloat)env.screenWidth, (GLfloat)env.screenHeight);
	}
}

void SaltAndBone::colorHint(int i)
{
	if(blockFail[i]){
		if(blockFail[i] & 1){
			if(!(blockFail[i] & 2)){
				glColor4f(0.0, 1.0, 0.0, 0.7);
			}
		} else if(blockFail[i] & 2){
			glColor4f(1.0, 0.75, 0.5, 0.7);
		} else if(blockFail[i] & 4){
			if(blockFail[i] & 8) glColor4f(0.85, 1.0, 1.0, 0.7);
			else glColor4f(0.0, 1.0, 1.0, 0.7);
		} else if(blockFail[i] & 8){
			glColor4f(0.85, 0.85, 0.85, 0.7);
		}
	}
}

void SaltAndBone::drawHint(int i)
{
	if(blockFail[i]){
		if(blockFail[i] & 1){
			if(!(blockFail[i] & 2)){
				drawGlyph("low", 0, 400, 300, 55, 2*i);
			}
		} else if(blockFail[i] & 2){
			drawGlyph("high", 0, 400, 300, 55, 2*i);
		} else if(blockFail[i] & 4){
			if(blockFail[i] & 8) drawGlyph("air", 0, 400, 300, 55, 2*i);
			drawGlyph("air", 0, 400, 300, 55, 2*i);
		} else if(blockFail[i] & 8){
			drawGlyph("unblock", 0, 300, 500, 55, 2*i);
		}
	}
}

void SaltAndBone::drawHUD()
{
	globalAnnounce.clear();
	int currentRound = P[0]->rounds + P[1]->rounds + 1;
	int timeSecs = timer / 60;
	if(timeSecs < 10) glColor4f(1.0, 0.0, 0.0, 1.0);
	drawGlyph((timeSecs < 10 ? "0" : "") + to_string(timeSecs > 99 ? 99 : timeSecs), 700, 200, 0, 90, 1);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	for(unsigned int i = 0; i < P.size(); i++){
		if(P[i]->name.size()) drawGlyph(P[i]->name, 100+800*i, 600, 30, 40, 0+2*i);
		else drawGlyph(things[i]->pick()->name, 100+800*i, 600, 30, 40, 0+2*i);
		drawGlyph(to_string(things[i]->current.meter[0].value), 1500*i, 100, 10, 35, 0+2*i);
		drawGlyph(to_string(things[i]->current.meter[1].value), 1500*i, 100, 860, 35, 0+2*i);
		glPushMatrix();
			glTranslatef(100+i*1000, 0, 0);
			colorHint(i);
			drawHint(i);
		glPopMatrix();
		if(counterHit[i] > 0){
			glColor4f(1.0, 1.0, 0.5, 0.7);
			drawGlyph("Counter", 100+1000*i, 400, 200, 55, 2*i);
		} else if(punish[i] > 0){
			glColor4f(0.6, 0.6, 1.0, 0.7);
			drawGlyph("Punish", 100+1000*i, 400, 200, 55, 2*i);
		}
		glColor4f(1.0, 1.0, 1.0, 1.0);
		if(P[i]->record){
			glColor4f(0.5, 1.0, 1.0, 0.7);
			drawGlyph("Recording", 100+1000*i, 600, 200, 55, 2*i);
			glColor4f(1.0, 1.0, 1.0, 1.0);
		}
		if(combo[(i+1)%2] < 0 || combo[i] || knockdown[i]){
			glColor4f(1.0, 1.0-.5*illegit[i], 1.0-.5*illegit[i], 1.0);
			if(combo[i] > 1)
				drawGlyph(to_string(combo[i]) + " hits", 100+800*i, 600, 400, 75, 0+2*i);
			if(counterHit[i] > 0)
				glColor4f(1.0, 1.0, 0.5, 0.8);
			else if(punish[i] > 0)
				glColor4f(0.6, 0.6, 1.0, 0.8);
			else if(blockFail[(i+1)%2])
				colorHint((i+1)%2);
			else if(combo[i] < 0)
				glColor4f(1.0, 1.0, 1.0, 0.2);
			else if(!knockdown[i])
				glColor4f(1.0, 1.0-.5*illegit[i], 1.0-.5*illegit[i], 0.8);
			if(damage[i] != 0 || knockdown[i])drawGlyph(to_string(damage[i]) + " damage", 100+800*i, 600, 475, 35, 0+2*i);
			if(knockdown[i]){
				if(knockdown[i] == 1) glColor4f(0.6, 1.0, 0.6, 1.0);
				drawGlyph("Knockdown", 100+800*i, 600, 510, 35, 0+2*i); 
			}
			glColor4f(1.0, 1.0, 1.0, 1.0);
		}
	}

	if(timer > 100 * 60 && timer < 100 * 60 + 31){ 
		if(killTimer) globalAnnounce("Eleven");
		else globalAnnounce("Round " + to_string(currentRound));
	}
	if(timer > 99 * 60 && timer < 99 * 60 + 31){ 
		globalAnnounce("FIGHT");
	}
	if(roundEnd && endTimer > 5 * 60 - 31){ 
		if(things[0]->current.meter[0].value > 0 && things[1]->current.meter[0].value > 0){
			globalAnnounce("TIME OUT");
		} else {
			globalAnnounce("DOWN");
		}
	}
	if(endTimer > 3 * 60 + 29 && endTimer < 4 * 60){ 
		if(things[0]->current.meter[0].value > things[1]->current.meter[0].value){ 
			drawGlyph(P[0]->pick()->name, 0, env.screenWidth, 300, 150, 1);
			drawGlyph("Wins", 0, env.screenWidth, 450, 150, 1);
		} else if(things[1]->current.meter[0].value > things[0]->current.meter[0].value){
			drawGlyph(P[1]->pick()->name, 0, env.screenWidth, 300, 150, 1);
			drawGlyph("Wins", 0, env.screenWidth, 450, 150, 1);
		} else if(things[0]->current.meter[0].value <= 0){ 
			globalAnnounce("Double KO");
		} else {
			globalAnnounce("Draw");
		}
	}
	glPushMatrix();
	globalAnnounce.draw(this);
	glPopMatrix();
	drawMeters();
}

void fightingGame::drawMeters()
{
	glDisable( GL_TEXTURE_2D );

	for(player *i:P){
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glPushMatrix();
			glTranslatef(env.screenWidth/2, 0, 0);
			glPushMatrix();
				glScalef(env.screenWidth/2 * (i->ID == 1 ? -1 : 1), env.screenHeight, 0);
				glDisable( GL_TEXTURE_2D );
				for(int j = 0; j < numRounds; j++){
					if(i->rounds > j) glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
					else glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
					float x = 1.0 / 8.0 + (float)j * .03, y = 2.0 / 75.0, w = .025, h = 1.0 / 90.0;
					glRectf(x, y, w + x, h + y);
				}
				for(HUDMeter <int> j:i->pick()->drawMeters(i->ID, i->current)){
					j.draw();
				}
				glEnable( GL_TEXTURE_2D );
			glPopMatrix();
		glPopMatrix();
		glFlush();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
	glEnable( GL_TEXTURE_2D );
}

void menu::draw()
{
	glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
	glRectf(0.0, 0.0, (GLfloat)game->env.screenWidth, (GLfloat)game->env.screenHeight);
	glEnable( GL_TEXTURE_2D );
	for(unsigned int i = 0; i < labels.size(); i++){
		glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(cursor == (int)i)*0.4);
		game->drawGlyph(labels[i], 0, game->env.screenWidth, game->env.screenHeight/2 - ((float)labels.size()/2.0) * 60 + 60*i, 60, 1);
	}
	glDisable( GL_TEXTURE_2D );
	glColor4f(1.0, 1.0, 1.0, 1.0f);
}

vector<HUDMeter<int>> character::drawMeters(int ID, status &current)
{
	vector<HUDMeter<int>> ret = current.meter;
	int hidden = 0;
	if(current.move){
		hidden = current.move->hidesMeter ? current.move->cost : 0;
	}

	ret[1].value += hidden;
	if(hidden) ret[4].value = 0;
	ret[4].x = ret[1].x + ret[1].w * (double)ret[1].value / (double)ret[1].maximum;

	ret[1].R = ret[1].value < ret[1].maximum / 2 ? 0.7 : 0;
	ret[1].G = ret[1].value == ret[1].maximum ? 1.0 : 1.0 * ((ret[1].value / (ret[1].maximum / 4) % 2));
	ret[1].B = ret[1].value < ret[1].maximum ? 1.0 : 0.0;
	ret[4].R = ret[1].R;
	ret[4].G = ret[1].G;
	ret[4].B = ret[1].B;

	ret[2] = ret[0];
	ret[0].R = 0;
	ret[0].G = 0;
	ret[0].B = 0;
	ret[0].A = .4;
	ret[0].value = ret[0].maximum;

	ret[3] = ret[1];
	ret[1].R = 0;
	ret[1].G = 0;
	ret[1].B = 0;
	ret[1].A = .4;
	ret[1].value = ret[1].maximum;
	return ret;
}

void instance::drawBoxen()
{
	float tColor = (current.throwInvuln == 0) * .6 + .4;
	glColor4f(tColor, tColor, tColor, 0.7f);
	glPushMatrix();
		glTranslatef(current.collision.x, -current.collision.y, 0);
		glRectf(0.0f, 0.0f, (GLfloat)(current.collision.w), (GLfloat)(-current.collision.h));
	glPopMatrix();
	for(unsigned int i = 0; i < current.hitreg.size(); i++){
		glFlush();
		glColor4f(CHState() ? 1.0f : 0.0f, ID == 2 ? 0.0f : 1.0f, ID == 2 ? 1.0f : 0.0f, 0.7f);
		glNormal3f(1.0f, 0.0f, 1.0f);
		glPushMatrix();
			glTranslatef(current.hitreg[i].x, -current.hitreg[i].y, 1);
			glRectf(0.0f, 0.0f, (GLfloat)(current.hitreg[i].w), (GLfloat)(-current.hitreg[i].h));
		glPopMatrix();
	}
	for(unsigned int i = 0; i < current.move->hitbox[current.frame].size(); i++){
		glFlush();
		glColor4f(1.0f, ID == 2 ? 1.0 : 0.0, 0.0, 0.7f);
		glPushMatrix();
			glTranslatef(current.move->hitbox[current.frame][i].x*current.facing + current.posX, -(current.move->hitbox[current.frame][i].y + current.posY), 0);
			glRectf(0.0f, 0.0f, (GLfloat)(current.move->hitbox[current.frame][i].w*current.facing), (GLfloat)(-(current.move->hitbox[current.frame][i].h)));
		glPopMatrix();
	}
	glFlush();
	glDisable( GL_TEXTURE_2D );
}

void instance::draw(GLint p)
{
	bool sCheck = spriteCheck();
	status * n;
	if(save.facing && particleType == 1 && current.freeze && current.counter) n = &save;
	else n = &current;

	if(n == &current){
		n->drawX = n->posX;
		n->drawY = n->posY;

		if(n->move->offX != 0) n->drawX += n->move->offX*n->facing;
		else{
			if(n->facing == 1){
				if(n->collision.x < n->drawX) n->drawX = n->collision.x;
				for(unsigned int i = 0; i < n->hitreg.size(); i++)
					if(n->hitreg[i].x < n->drawX) n->drawX = n->hitreg[i].x;
				for(unsigned int i = 0; i < n->hitbox.size(); i++)
					if(n->hitbox[i].x < n->drawX) n->drawX = n->hitbox[i].x;
			} else {
				if(n->collision.x + n->collision.w > n->drawX) n->drawX = n->collision.x + n->collision.w;
				for(unsigned int i = 0; i < n->hitreg.size(); i++)
					if(n->hitreg[i].x + n->hitreg[i].w > n->drawX) n->drawX = n->hitreg[i].x + n->hitreg[i].w;
				for(unsigned int i = 0; i < n->hitbox.size(); i++)
					if(n->hitbox[i].x + n->hitbox[i].w > n->drawX) n->drawX = n->hitbox[i].x + n->hitbox[i].w;
			}
		}
		if(n->move->offY != 0) n->drawY += n->move->offY;
		else{
			if(n->collision.y < n->drawY) n->drawY = n->collision.y;
			for(unsigned int i = 0; i < n->hitreg.size(); i++)
				if(n->hitreg[i].y < n->drawY) n->drawY = n->hitreg[i].y;
			for(unsigned int i = 0; i < n->hitbox.size(); i++)
				if(n->hitbox[i].y < n->drawY) n->drawY = n->hitbox[i].y;
		}
	}
	glEnable(GL_TEXTURE_2D);
	if(sprite && sCheck){
		//	glColor4f(0.75f, 0.5f, 0.85f, 1.0f);
		glPushMatrix();
			glTranslatef(n->drawX, -n->drawY, 0);
			glPushMatrix();
				glScalef((float)n->facing, 1.0, 1.0);
				pick()->draw(*n, p);
			glPopMatrix();
		glPopMatrix();
	}
	glDisable(GL_TEXTURE_2D);
	if(!sCheck || boxen){
		if(!n->move || n->frame > n->move->frames){
			glPushMatrix();
				glTranslatef(current.drawX, -current.drawY, 0);
				glPushMatrix();
					glScalef((float)n->facing, 1.0, 1.0);
					pick()->neutral->drawBoxen(0);
				glPopMatrix();
			glPopMatrix();
		} else drawBoxen();
	}
}

void player::drawHitParticle()
{
	/*Stand-in for now, just to indicate block type*/
	if(particleLife > 0){
		switch (particleType){
		case 1:
			if(current.opponent->current.counter == 1) glColor4f(1.0f, 0.5f, 0.0f, 0.7f);
			else glColor4f(1.0f, 0.0f, 0.0f, 0.7f);
			break;
		case 0:
			glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
			break;
		case -1:
			glColor4f(1.0f, 1.0f, 1.0f, 0.7f);
			break;
		case -2:
			glColor4f(1.0f, 1.0f, 0.0f, 0.7f);
			break;
		case -5:
			glColor4f(0.4f, 0.4f, 0.4f, 0.5f);
		}
		glPushMatrix();
			glTranslatef(current.posX, 0.0f, 0.0f);
			glPushMatrix();
				glTranslatef(0.0f, -current.collision.y, 0.0f);
				glRectf((GLfloat)(-10*current.facing), (GLfloat)(-current.collision.h), (GLfloat)(50 * current.facing), (GLfloat)(-current.collision.h - 40));
			glPopMatrix();
			for(SDL_Rect i:hitLocation){
				glPushMatrix();
					glTranslatef(i.x, -current.posY - i.y - i.h, 0.0f);
					glRectf(0.0f, 0.0f, i.w, i.h);
				glPopMatrix();
			}
		glPopMatrix();
		particleLife--;
	} else {
		blockType = 0;
		hitLocation.clear();
	}
}

void avatar::draw(status &current, GLint p)
{
	if(palette) glUseProgram(p);
	GLint paletteID = glGetUniformLocation(p, "palette"); //Get a pointer to a shader uniform var
	glActiveTexture(GL_TEXTURE2); // Choose texture unit 1 (base texture). 
	glBindTexture(GL_TEXTURE_2D, palette); //Bind texture as normal
	glUniform1i(paletteID, 2); //Set base texture sampler uniform var
	current.move->draw(current.frame, p);
	glUseProgram(0);
}

void words::draw(fightingGame* game)
{
	float sw = game->env.screenWidth, sh = game->env.screenHeight;
	game->drawGlyph(text, x*sw, w*sw, y*sh, h*sh, align);
}

void fightingGame::drawGlyph(string s, int x, int totalSpace, int y, int height, int just)
{
	int w, h, width = 0, padding = 0, totalWidth = 0;
	if(just != 0){
		for(char c : s){
			if(c == ' ') totalWidth += w * sf / 2;
			else if(c == '\0');
			else{
				glBindTexture(GL_TEXTURE_2D, glyph[toupper(c)]);
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
				sf = (float)height / (float)h;
				totalWidth += w * sf;
			}
		}
		if(just == 2) padding = totalSpace - totalWidth;
		else padding = (totalSpace - totalWidth) / 2;
	}

	float sf;
	for(char c : s){
		if(c == ' ') x += (float)width / 2.0;
		else{
			glBindTexture(GL_TEXTURE_2D,glyph[toupper(c)]);

			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
			sf = (float)height / (float)h;
			width = w * sf;
			glPushMatrix();
				glTranslatef(x + padding, y, 0.f);
				glBegin(GL_QUADS);
				glTexCoord2i(0, 0);
				glVertex3f(0, 0, 0.f);

				glTexCoord2i(1, 0);
				glVertex3f(width, 0, 0.f);

				glTexCoord2i(1, 1);
				glVertex3f(width, height, 0.f);

				glTexCoord2i(0, 1);
				glVertex3f(0, height, 0.f);
				glEnd();
			glPopMatrix();
			x += (float)width;
		}
	}
}

void action::draw(int f, GLint p)
{
	if(modifier && basis.move) basis.move->draw(basis.frame, p);
	if(sprite[f]){
		GLint baseTextureID = glGetUniformLocation(p, "colorIn"); //Get a pointer to a shader uniform var
		glActiveTexture(GL_TEXTURE0); // Choose texture unit 0 (base texture). 
		glBindTexture(GL_TEXTURE_2D, sprite[f]); //Bind texture as usual
		glUniform1i(baseTextureID, 0); //Set base texture sampler uniform var
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex3f(0.0f, (GLfloat)(-height[f]), 0.f);

		glTexCoord2i(1, 0);
		glVertex3f((GLfloat)(width[f]), (GLfloat)(-height[f]), 0.f);

		glTexCoord2i(1, 1);
		glVertex3f((GLfloat)(width[f]), 0.0f, 0.f);

		glTexCoord2i(0, 1);
		glVertex3f(0.0f, 0.0f, 0.f);
		glEnd();
	}
}

bool instance::spriteCheck()
{
	if(current.move) return current.move->spriteCheck(current.frame);
	else return 0;
}
bool avatar::spriteCheck(action *& cMove, int f)
{
	if(cMove == nullptr) return 0;
	else return cMove->spriteCheck(f);
}

bool action::spriteCheck(int f)
{
	if(modifier && basis.move) basis.move->spriteCheck(basis.frame);
	if(sprite.empty()) return 0;
	else if(sprite[f] != 0) {
		return 1;
	}
	else return 0;
}

void SaltAndBone::writeImage(string movename, int frame, action * move)
{
	int Y = 0;
	int X = 0;
	int maxY = move->collision[frame].y + move->collision[frame].h,
	    maxX = move->collision[frame].x + move->collision[frame].w;
	for(unsigned int i = 0; i < move->hitreg[frame].size(); i++){
		if(move->hitreg[frame][i].y < Y)
			Y = move->hitreg[frame][i].y;
		if(move->hitreg[frame][i].x < X)
			X = move->hitreg[frame][i].x;
		if(move->hitreg[frame][i].x + move->hitreg[frame][i].w > maxX)
			maxX = move->hitreg[frame][i].x + move->hitreg[frame][i].w;
		if(move->hitreg[frame][i].y + move->hitreg[frame][i].h > maxY)
			maxY = move->hitreg[frame][i].y + move->hitreg[frame][i].h;
	}
	for(unsigned int i = 0; i < move->hitbox[frame].size(); i++){
		if(move->hitbox[frame][i].y < Y) 
			Y = move->hitbox[frame][i].y;
		if(move->hitbox[frame][i].x < X) 
			X = move->hitbox[frame][i].x;
		if(move->hitbox[frame][i].x + move->hitbox[frame][i].w > maxX)
			maxX = move->hitbox[frame][i].x + move->hitbox[frame][i].w;
		if(move->hitbox[frame][i].y + move->hitbox[frame][i].h > maxY)
			maxY = move->hitbox[frame][i].y + move->hitbox[frame][i].h;
	}
	int w = maxX + X;
	int h = maxY + Y;
	int x = 0;
	int y = 0;
	if(Y < 0){ 
		h -= Y;
		y = -Y;
	}
	if(X < 0){
		w -= X;
		x = -X;
	}
screenInit(w, h);



	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glRectf(0.0f, 0.0f, (GLfloat)w, (GLfloat)h);

	glPushMatrix();
		glTranslatef(-x, -y, 0.0);
		move->drawBoxen(frame);
	glPopMatrix();

}

void action::drawBoxen(int frame){
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	glRectf((GLfloat)(collision[frame].x), (GLfloat)(collision[frame].y), (GLfloat)(collision[frame].x + collision[frame].w), (GLfloat)(collision[frame].y + collision[frame].h));
	for(unsigned int i = 0; i < hitreg[frame].size(); i++){
		glFlush();
		glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
		glRectf((GLfloat)(hitreg[frame][i].x), (GLfloat)(hitreg[frame][i].y), (GLfloat)(hitreg[frame][i].x + hitreg[frame][i].w), (GLfloat)(hitreg[frame][i].y + hitreg[frame][i].h));
	}
	for(unsigned int i = 0; i < hitbox[frame].size(); i++){
		glFlush();
		glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
		glRectf((GLfloat)(hitbox[frame][i].x), (GLfloat)(hitbox[frame][i].y), (GLfloat)(hitbox[frame][i].x + hitbox[frame][i].w), (GLfloat)(hitbox[frame][i].y + hitbox[frame][i].h));
	}
	glFlush();
}

bool fightingGame::screenInit(int w, int h)
{
	/*Initialize SDL*/
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) return false;
	/*WM stuff*/
	if(window::screen){ 
		SDL_FreeSurface(screen);
		screen = nullptr;
	}
	SDL_WM_SetCaption("GUFG", "GUFG");
	if((screen = SDL_SetVideoMode(w, h, 32, SDL_OPENGL)) == nullptr)
		return false;
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

void session::draw(model & object)
{
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); 
	glPushMatrix(); 
	glPushMatrix(); 
	glMatrixMode( GL_PROJECTION );
	gluPerspective(100.0, 1.5, 1.0, 150.0);
	float modelColor[] = { r, g, b, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, modelColor);
	glMatrixMode( GL_MODELVIEW );
	glTranslatef(camX, camY, camZ);
	glPushMatrix(); 
	glRotatef(spin, 0.0, 0.0, 1.0f);
	glPushMatrix(); 
	glRotatef(pan, 0.0, 1.0, 0.0f);
	glPushMatrix(); 
	glRotatef(tilt, 1.0, 0.0, 0.0f);
	glPushMatrix();
	object.draw();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix(); 
	SDL_GL_SwapBuffers();
}

void model::draw(GLint shaderProg)
{
/* Load the identity matrix into modelmatrix. rotate the model, and move it back 5 */
	vect v;
	for(unsigned int i = 0; i < faces.size(); i ++){
		v.unitNormal( vertices[0][faces[i][0]-1], vertices[1][faces[i][0]-1], vertices[2][faces[i][0]-1],
				vertices[0][faces[i][1]-1], vertices[1][faces[i][1]-1], vertices[2][faces[i][1]-1],
				vertices[0][faces[i][2]-1], vertices[1][faces[i][2]-1], vertices[2][faces[i][2]-1]);
		switch (faces[i].size()){
		case 3:
			glBegin(GL_TRIANGLES);
				glNormal3f(v.x, v.y, v.z);
				glVertex3f(vertices[0][faces[i][0]-1], vertices[1][faces[i][0]-1], vertices[2][faces[i][0]-1]);
				glVertex3f(vertices[0][faces[i][1]-1], vertices[1][faces[i][1]-1], vertices[2][faces[i][1]-1]);
				glVertex3f(vertices[0][faces[i][2]-1], vertices[1][faces[i][2]-1], vertices[2][faces[i][2]-1]);
			glEnd();
			break;
		case 4:
			glBegin(GL_QUADS);
				glNormal3f(v.x, v.y, v.z);
				glVertex3f(vertices[0][faces[i][0]-1], vertices[1][faces[i][0]-1], vertices[2][faces[i][0]-1]);
				glVertex3f(vertices[0][faces[i][1]-1], vertices[1][faces[i][1]-1], vertices[2][faces[i][1]-1]);
				glVertex3f(vertices[0][faces[i][2]-1], vertices[1][faces[i][2]-1], vertices[2][faces[i][2]-1]);
				glVertex3f(vertices[0][faces[i][3]-1], vertices[1][faces[i][3]-1], vertices[2][faces[i][3]-1]);
			glEnd();
			break;
		}
	}
}
