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
{ // In other words, next task is make this class be able to play a sample (cut to a frame), play an array, play a frame of silence, and play an Ogg file (cut to a frame). 
public:
    fragment(array<unsigned int, sampleRate / 60>);//For testing purposes
    fragment(const sample&); //Should cut off everything past sampleRate/60 samples.
    fragment() {}
    fragment(string, string="Ogg");
};

template <typename T_func>
class synth : public sample
{
    ALuint operator()(); //Generate one frame worth of music according to a mathematical rule. T is evaluated "once per sample rate" (but untimed) to populate the ALuint with one frame of sound
};

class vocalization : array<vector<fragment>, 3>
{
public: 
    vocalization(const sample&, int, int); //Chop up sample. Load the first n frames as attack and the last m frames as decay.
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
