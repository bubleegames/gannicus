#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <string>
#include <vector>
#include <array>
#include "notation.h"

using std::array;
using std::string;
using std::vector;

#ifndef OPERA_INCLUDED
#define OPERA_INCLUDED

const int sampleRate = 8040;

class sample {
public:
	ALuint buffer;
	ALuint operator()();
	sample(const sample&);
	sample() {}
	sample(string);
};

class fragment : public sample //Class corresponding to a frame of audio.
{
public:
    fragment(array<unsigned int, sampleRate / 60>);
    fragment(const sample&); //Should cut off everything past sampleRate/60 samples.
};

class vocalization : array<vector<fragment>, 3>
{
public: 
    vocalization(vector<fragment>, vector<fragment>, vector<fragment>); //Attack, sustain, and decay fragments, each of which may be n fragments long.
    void play(int, int); //Attack, then loop sustain for duration - S, then decay.
                         //Arguments are volume and duration
    void stop(); //Stop even if still decaying.
    ~vocalization();
};

class voice : vector<array<vocalization, ChromaticOctave>>
{
    pitch root;
public:
    voice(pitch, vector<array<vocalization, ChromaticOctave>>);//Pitch argument is root pitch for the voice
    void vocalize(note); //Playback the mapped note if a vocalization exists for it
};
  
class soundScape {
public:
	ALuint source[16];
	virtual void play(int);
	virtual void init();
	virtual ~soundScape();
};
  
#endif
