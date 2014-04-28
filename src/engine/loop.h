#ifndef ___loop
#define ___loop

#include <chrono>

using std::ratio;
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
	game.createPlayers(2);
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

#endif
