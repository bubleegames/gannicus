#include "analytics.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctime>
void script::init(int players)
{
	for(unsigned int i = 0; i < players; i++){
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
	return genEvent(0, 0, l);
}

bool script::genEvent(int p, int f, frame &t)
{
	if(command.size() == 0 || p >= command.size()-1){ 
		printf("Too little p\n");
		return 0;
	}
	if(command[p].size() == 0 || f >= command[p].size()-1){
		printf("Too little f\n");
		return 0;
	}
	for(int i = 0; i < 4; i++) t.axis[i] = command[p][f].axis[i];
	printf("%i", 5-3*t.axis[0]+3*t.axis[1]-t.axis[2]+t.axis[3]);
	for(int i = 0; i < command[p][f].pos.size(); i++){
		t.pos[i] = !(!(command[p][f].pos[i]));
		t.neg[i] = command[p][f].neg[i];
		if(t.pos[i]) printf("%c", 'A'+i);
		else if(t.neg[i]) printf("]%c[", 'A'+i);
	}
	printf("\n");
	return 1;
}

script::script(char* filename)
{
	load(filename);
}

void script::load(char* filename)
{
	std::ifstream read;
	read.open(filename);
	printf("%s\n", filename);
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
			bool a;
			for(int j = 0; j < 4; j++){
				read >> a;
				temp.axis.push_back(a);
			}
			int p;
			for(int j = 0; j < buttons; j++){
				read >> p;
				temp.pos.push_back(p);
			}
			bool n;
			for(int j = 0; j < buttons; j++){
				read >> n;
				temp.neg.push_back(n);
			}
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
			for(unsigned int k = 0; k < command[j][i].axis.size(); k++) scribe << command[j][i].axis[k] << " ";
			for(unsigned int k = 0; k < command[j][i].pos.size(); k++) scribe << command[j][i].pos[k] << " ";
			for(unsigned int k = 0; k < command[j][i].neg.size(); k++) scribe << command[j][i].neg[k] << " ";
			scribe << '\n';
		}
	}
	scribe << '\n';
	scribe.close();
}

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
	int check;
	char buffer[500];
	char* token;
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
		for(int j = 0; j < wins.size(); j++){
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
