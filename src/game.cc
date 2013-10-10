#include "engine/interface.h"
#include <chrono>
#include <iostream>

using std::ratio;
using std::vector;
using std::string;
using std::cout;
using namespace std::chrono;

template<typename Engine, size_t FPS, size_t Rounds>
struct loop
{
	typedef duration<float,ratio<1,FPS>> frame_t;
	Engine game;
	loop();
	bool advance();
	void run();
};

template <typename Engine, size_t FPS, size_t Rounds>
loop<Engine, FPS, Rounds>::loop()
{
	game.screenInit();
	game.createPlayers();
	game.loadMisc();
	game.startGame();
	game.numRounds = Rounds;
}

template<typename Engine, size_t FPS, size_t Rounds>
bool loop<Engine, FPS, Rounds>::advance()
{
	game.readInput();
	game.resolve();
	game.draw();
	game.cleanup();
	return !game.gameover;
}

template<typename Engine, size_t FPS, size_t Rounds>
void loop<Engine, FPS, Rounds>::run()
{
	bool keepGoing = true;
	high_resolution_clock::time_point start;
	while (keepGoing)
	{
		start = high_resolution_clock::now();
		keepGoing = advance();
		frame_t elapsed;
		do elapsed = duration_cast<frame_t>
			(high_resolution_clock::now() - start);
		while (elapsed.count() < 1);
	}
}

int main(int argc, const char* argv[])
{
	vector <string> args;
	for(int i = 0; i < argc; i++) args.push_back(argv[i]);
	loop<SaltAndBone, 120, 2> game;
	game.game.handleArgs(args);
	game.run();
	return 0;
}

