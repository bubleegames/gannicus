#include "engine/interface.h"
#include "engine/loop.h"
#include <iostream>

using std::vector;
using std::string;

int main(int argc, const char* argv[])
{
	vector <string> args;
	for(int i = 0; i < argc; i++) args.push_back(argv[i]);
	loop<interface, 60, 2> game;
	game.game.handleArgs(args);
	game.run();
	return 0;
}

