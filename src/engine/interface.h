/*Copyright Somnambulant Studios 2012-2013*/
#include "harness.h"
#include "window.h"
#include "opera.h"
#include "environment.h"
#include "force.h"
#include "menu.h"
#include <map>
#ifndef ___SaltAndBone
#define ___SaltAndBone
using std::string;
using std::vector;
using std::map;
class resolver {
	virtual void resolve() = 0;
	virtual void inputTransform() = 0;
};

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
	virtual void roundInit() {}
	virtual void matchInit() {}
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
	map<int, character*> charTable;
	vector<string> characterManifest;
	character * generateCharacter(int);
	int numRounds;
	int numChars;
	bool demo:1;
	bool screenShot:1;
	Mix_Music *matchMusic;
	environment env;
};

class SaltAndBone : public fightingGame{
public:
	SaltAndBone();
	~SaltAndBone();
	void readInput();
	void loadMisc();
	void initCharacters();
	void initCharTable();
	virtual void roundInit();
	virtual void matchInit();
	virtual void initShaders();
	virtual void loadAssets();
	virtual void handleArgs(vector<string>);
	virtual void comboScaling(hStat&, int);
	void processInput(SDL_Event&);
	void createPlayers(int);
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
	void cSelectMenu();
	character * generateCharacter(int);
	void mainMenu(int);
	void keyConfig(int);
	void doRematchMenu();
	void doMenu(menu&);
	void checkWin();
	void dragBG(int);
	void doSuperFreeze();
	void readMatchupChart();
	void writeMatchupChart();
	void writeImage(string, int, action*);

	sample call[2];
	int configMenu[2];
	int mMenu[2];
	bool firstFrame:1;
	bool shortcut:1;
	bool continuous:1;
	bool single:1;
	bool analytics:1;
	menu pauseMenu;
	menu rematchMenu;
	chart *stats;

	int musicVolume;
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
