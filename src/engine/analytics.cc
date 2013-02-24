#include "analytics.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctime>
void script::init(int players)
{
	for(int i = 0; i < players; i++){
		selection.push_back(-1);
		std::vector<frame> tvec;
		command.push_back(tvec);
	}
}

void script::init(std::vector<int> s)
{
	for(unsigned int i = 0; i < s.size(); i++){
		selection.push_back(s[i]);
		std::vector<frame> tvec;
		command.push_back(tvec);
	}
}

bool script::test()
{
	frame l;
	for(int i = 0; i < 4; i++)
		l.axis.push_back(0);
	for(int i = 4; i < 6; i++){
		l.pos.push_back(0);
		l.neg.push_back(0);
	}
	l.n.i = 0;
	l.n.raw.dir = 5;
	return genEvent(0, 0, l);
}

bool script::genEvent(int p, int f, frame &t)
{
	if(command.empty() || (unsigned int)p >= command.size()) return 0;
	if(command[p].empty() || (unsigned int)f >= command[p].size()) return 0;

	for(bool i:t.axis) i = 0;
	if(command[p][f].n.raw.dir > 6){
		t.axis[0] = true;
		t.axis[1] = false;
	} else if(command[p][f].n.raw.dir < 4){
		t.axis[1] = true;
		t.axis[0] = false;
	}
	if(command[p][f].n.raw.dir % 3 == 1){
		t.axis[2] = true;
		t.axis[3] = false;
	} else if(command[p][f].n.raw.dir % 3 == 0) {
		t.axis[3] = true;
		t.axis[2] = false;
	}

	for(int i = 0; i < 5; i++){
		if(command[p][f].n.i & (1 << (i*2+4))){
			t.pos[i] = 1;
			t.neg[i] = 0;
		} else if(command[p][f].n.i & (1 << (i*2+5))) {
			t.pos[i] = 0;
			t.neg[i] = 1;
		} else {
			t.pos[i] = 0;
			t.neg[i] = 0;
		}
	}
	if(command[p][f].n.i != 5) printf("%i: %i\n", p, command[p][f].n.i);

	if(command[p][f].n.raw.Start) t.pos[5] = 1;
	else t.pos[5] = 0;
	return 1;
}

void script::push(frame t)
{
	push(0, t);
}

void script::push(int p, frame t)
{
	t.n.i = 0;
	if(t.pos[0] == 1) t.n.raw.A = 1;
	if(t.neg[0] == 1) t.n.raw.A = -1;
	if(t.pos[1] == 1) t.n.raw.B = 1;
	if(t.neg[1] == 1) t.n.raw.B = -1;
	if(t.pos[2] == 1) t.n.raw.C = 1;
	if(t.neg[2] == 1) t.n.raw.C = -1;
	if(t.pos[3] == 1) t.n.raw.D = 1;
	if(t.neg[3] == 1) t.n.raw.D = -1;
	if(t.pos[4] == 1) t.n.raw.E = 1;
	if(t.neg[4] == 1) t.n.raw.E = -1;
	if(t.pos[5] == 1) t.n.raw.Start = true;
	else t.n.raw.Start = false;

	t.n.raw.dir = 5;
	if(t.axis[0]) t.n.raw.dir += 3;
	if(t.axis[1]) t.n.raw.dir -= 3;
	if(t.axis[2]) t.n.raw.dir--;
	if(t.axis[3]) t.n.raw.dir++;

	printf("%i: %i\n", p, t.n.i);
	command[p].push_back(t);
}

script::script(char* filename)
{
	load(filename);
}

void script::load(char* filename)
{
	std::ifstream read;
	read.open(filename);
	if(read.fail()){
		init(1); return;
	}
	int players, buttons, s;
	read >> players; read >> buttons;
	for(int i = 0; i < players; i++){
		read >> s;
		selection.push_back(s);
	}
	for(int i = 0; i < players; i++){
		std::vector<frame> t;
		command.push_back(t);
	}
	while(!read.eof()){
		for(int i = 0; i < players; i++){
			frame temp;
			read >> temp.n.i;
			command[i].push_back(temp);
		}
	}
}

void script::write()
{
	write(NULL);
}

void script::write(char * name)
{
	std::ofstream scribe;
	if(name) scribe.open(name);
	else{
		char buffer [200];
		time_t now = time(0);
		tm* localTime = localtime(&now);
		sprintf(buffer, ".data/replays/%s", asctime(localTime));
		scribe.open(buffer);
	}
	if(command[0].size() == 0){
		scribe.close();
		return;
	}
	scribe << selection.size() << " " << command[0][0].pos.size() << '\n';
	for(unsigned int i = 0; i < selection.size(); i++) 
		scribe << selection[i] << " ";
	scribe << '\n';
	for(unsigned int i = 0; i < command[0].size(); i++){
		for(unsigned int j = 0; j < command.size(); j++){
			scribe << command[j][i].n.i << '\n';
		}
	}
	scribe << '\n';
	scribe.close();
}

script::~script(){}

chart::chart(int size)
{
	std::vector<int> row;
	for(int i = 0; i < size + 1; i++){
		for(int j = 0; j < size + 1; j++){
			row.push_back(0);
		}
		wins.push_back(row);
	}
}

float chart::matchup(int a, int b)
{
	return (float)wins[a][b] / ((float)wins[b][a] + (float)wins[a][b]);
}

void chart::recordWin(int a, int b)
{
	wins[a][b]++;
}

void chart::init()
{
	std::ifstream read;
	unsigned int check;
	bool fresh = false;
	read.open(".data/.charts.csv");
	if(read.fail()) fresh = true;
	read >> check;
	if(check != wins.size()) fresh = true;
	if(!fresh){
		for(unsigned int i = 0; i < wins.size(); i++){
			read.ignore(200, ',');
			for(unsigned int j = 0; j < wins.size(); j++){
				read >> wins[i][j];
			}
		}
	}
	read.close();
}

void chart::write()
{
	std::ofstream wr;
	wr.open(".data/.charts.csv");
	wr << wins.size() << '\n' << " ";
	for(unsigned int i = 0; i < wins.size(); i++){
		wr << ",";
		wr << i;
	}
	wr << "\n";
	for(unsigned int i = 0; i < wins.size(); i++){
		wr << i;
		for(unsigned int j = 0; j < wins.size(); j++){
			wr << ",";
			if(i == j) wr << "-";
			else wr << wins[i][j];
		}
		wr << "\n";
	}
	wr.close();
}

chart::~chart()
{
	write();
}
