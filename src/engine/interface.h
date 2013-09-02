/*Copyright Somnambulant Studios 2012-2013*/
#include "harness.h"
#include "window.h"
#include "opera.h"
#ifndef ___interface
#define ___interface
using std::string;
using std::vector;

class gameInstance : public window, public arcadeHarness, public HUD, public soundScape {
public:
	virtual void resolve() = 0;
	virtual void handleArgs(vector<string>) {}
	virtual bool screenInit();
	virtual void init();
	virtual bool screenInit(int, int);
	virtual void processInput(SDL_Event&);
	virtual void initialConfig(int);
	virtual void unitCollision(instance*, instance*);
	virtual void genInput();
	virtual int drawGlyph(string, int, int, int, int, int);
	script *oldReplay;
	unsigned int replayIterator;
	virtual void print();

	vector<player*> P;

	int screenHeight, screenWidth, floor, wall;
	float prorate[2];
	bool pauseEnabled:1;
	bool scripting:1;
	float scalingFactor, sf;
	bool initd:1;
};

class interface : public gameInstance{
public:
	interface();
	~interface();
	void readInput();
	void loadMisc();
	void initCharacters();
	virtual void initShaders();
	virtual void loadAssets();
	virtual void handleArgs(vector<string>);
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
	void summonAttractors();
	void draw();
	void drawLoadingScreen();
	void drawHitParticles();
	void drawHUD();
	void drawHint(int);
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

	vector<bool> select;
	vector<int> selection;
	sample call[2];
	int menu[2];
	int configMenu[2];
	int rMenu;
	int pMenu;
	float bgR, bgG, bgB;
	SDL_Rect bg;
	SDL_Rect prox;
	bool firstFrame:1;
	bool shortcut:1;
	bool continuous:1;
	bool single:1;
	bool analytics:1;
	chart *stats;
	int numRounds;
	int grav;

	GLuint background;
	int musicVolume;
	Mix_Music *matchMusic;
	Mix_Music *menuMusic;
	Mix_Chunk **announceWinner;
	Mix_Chunk *announceRound[3];
	Mix_Chunk *announceDraw[2];
	Mix_Chunk *announceFight;
	Mix_Chunk *announceEnd[2];
	Mix_Chunk *announceSelect;

	vector<attractor *> globals;

	script *replay;

	int numChars;
	GLuint selectScreen; 
	SDL_Rect wheel;

	vector<vector<int> > buttons;
	int timer;
	bool roundEnd:1;
	bool killTimer:1;
	int endTimer;
};
#endif
