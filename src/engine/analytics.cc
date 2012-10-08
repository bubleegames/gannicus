#include "analytics.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctime>

frame::frame()
{
	next = NULL;
}

frame::frame(bool a[], int p[], bool n[])
{
	for(int i = 0; i < 5; i++){
		if(i < 4) axis[i] = a[i];
		pos[i] = p[i];
		neg[i] = n[i];
	}
	next = NULL;
}

void frame::cull()
{
	if(next){ 
		next->cull(); 
		delete next;
	}
	next = NULL;
}

void frame::append(frame * n)
{
	next = n;
}

replay::replay()
{
	start[0] = NULL;
	current[0] = NULL;
	fcounter = 0;
}

replay::replay(int p1, int p2)
{
	start[0] = NULL;
	current[0] = NULL;
	selection[0] = p1;
	selection[1] = p2;
	fcounter = 0;
}

void replay::append(frame * p1, frame * p2)
{
	if(!start[0]){ 
		start[0] = p1;
		start[1] = p2;
	} else { 
		current[0]->append(p1);
		current[1]->append(p2);
	}
	current[0] = p1;
	current[1] = p2;
	fcounter++;
}

void replay::load(const char* filename)
{
	printf("Stub function: This will load a replay file\n");
}

void replay::write()
{
	std::ofstream scribe;
	frame * iterator[2];
	for(int i = 0; i < 2; i++)
		iterator[i] = start[i];
	char buffer[100];
	time_t currtime;
	time(&currtime);
	sprintf(buffer, ".data/replays/%s.frp", asctime(localtime(&currtime)));
	scribe.open(buffer);
	scribe << selection[0] << " v " << selection[1] << "\n";
	do{
		for(int i = 0; i < 2; i++){
			for(int j = 0; j < 5; j++){
				if(j < 4) scribe << iterator[i]->axis[j] << " ";
				scribe << iterator[i]->pos[j] << " ";
				scribe << iterator[i]->neg[j] << " ";
			}
			iterator[i] = iterator[i]->next;
		}
		scribe << "\n";
	} while (iterator[0] != NULL);
	scribe.close();
}

replay::~replay()
{
	if(start[0]){
		start[0]->cull();
		start[1]->cull();
	}
}
