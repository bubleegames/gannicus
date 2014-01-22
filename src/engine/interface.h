/*Copyright Somnambulant Studios 2012-2013*/
#include "harness.h"
#include "window.h"
#include "opera.h"
#include "environment.h"
#include "force.h"
#ifndef ___SaltAndBone
#define ___SaltAndBone
using std::string;
using std::vector;

class fightingGame : public window, public arcadeHarness, public HUD, public soundScape {
public:
	virtual void resolve() = 0;
	virtual void handleArgs(vector<string>) {}
	virtual bool screenInit();
	virtual void init();
	virtual void drawMeters();
	virtual bool screenInit(int, int);
	virtual void processInput(SDL_Event&);
	virtual void initialConfig(int);
	virtual void comboScaling(hStat&, int) {}
	words globalAnnounce;
	virtual void unitCollision(instance*, instance*);
	virtual void genInput();
	virtual void drawGlyph(string, int, int, int, int, int);
	script *oldReplay;
	unsigned int replayIterator;
	virtual void print();

	vector<player*> P;
	float prorate[2];
	bool pauseEnabled:1;
	float scalingFactor, sf;
	bool scripting:1;
	bool initd:1;
	vector<bool> select;
	vector<int> selection;
	int numRounds;
	int numChars;
	environment env;
};

class SaltAndBone : public fightingGame{
public:
	SaltAndBone();
	~SaltAndBone();
	void readInput();
	void loadMisc();
	void initCharacters();
	virtual void initShaders();
	virtual void loadAssets();
	virtual void handleArgs(vector<string>);
	virtual void comboScaling(hStat&, int);
	void processInput(SDL_Event&);
	void createPlayers();
	void createPlayers(string);
	void startGame();
	void loadMatchBackground();

	void resolve();
	void resolveInputs();
	void resolvePhysics();
	void resolveCollision();
	void resolveCamera();
	void resolveCombos();
	void resolveHits();
	void resolveThrows();
	void resolveSummons();
	void summonForces();
	void draw();
	void drawLoadingScreen();
	void drawHitParticles();
	void drawHUD();
	void drawHint(int);
	void colorHint(int);
	void drawGame();
	void drawCSelect();
	void drawMainMenu(int);
	void drawConfigMenu(int);
	void drawRematchMenu();
	void drawPauseMenu();
	void cleanup();
	void runTimer();
	void roundInit();
	void matchInit();
	void cSelectMenu();
	void mainMenu(int);
	void keyConfig(int);
	void rematchMenu();
	void pauseMenu();
	void checkWin();
	void dragBG(int);
	void doSuperFreeze();
	void readMatchupChart();
	void writeMatchupChart();
	void writeImage(string, int, action*);

	sample call[2];
	int menu[2];
	int configMenu[2];
	int rMenu;
	int pMenu;
	bool firstFrame:1;
	bool shortcut:1;
	bool continuous:1;
	bool single:1;
	bool analytics:1;
	chart *stats;

	int musicVolume;
	Mix_Music *matchMusic;
	Mix_Music *menuMusic;
	Mix_Chunk **announceWinner;
	Mix_Chunk *announceRound[3];
	Mix_Chunk *announceDraw[2];
	Mix_Chunk *announceFight;
	Mix_Chunk *announceEnd[2];
	Mix_Chunk *announceSelect;

	script *replay;

	GLuint selectScreen; 
	SDL_Rect wheel;

	vector<vector<int> > buttons;
	int timer;
	bool roundEnd:1;
	bool killTimer:1;
	int endTimer;
};
#endif
