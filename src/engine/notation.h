#include<array>
#include<bitset>
#include<vector>

#ifndef ___notation
#define ___notation

using std::array;
using std::bitset;
using std::vector;

enum diatonic {Tonic, Supertonic, Mediant, Subdominant, 
               Dominant, Submediant, Leading, DiatonicDegrees};

enum chromatic {Cr0,Cr1,Cr2,Cr3,Cr4,Cr5,Cr6,Cr7,
                     Cr8,Cr9,Cr10,Cr11,ChromaticOctave};

enum accidental {DoubleFlat=-2, Flat, Neutral, Sharp, DoubleSharp};

typedef array <chromatic, DiatonicDegrees> scale;

struct key
{
    diatonic m; //Mode/starting point in the scale
    scale s; //Scale basis
    chromatic f; //Fundamental
};

struct note
{
	accidental a;
	diatonic d;
	size_t octave; //Octave is between like, 0 and 5
	size_t dynamic; //I think we should use a narrow range of "dynamics" that get applied to overall volume at a different step, e.g. dynamic is probably between like, 0 and 5
	size_t duration; //In frames
};

class phrase : vector<note> {};
//methods: mode (output in different mode), modulate (output with root transposed), key (output in different Key), scale, etc

template <size_t FrameLength>
class rhythm : vector<array<size_t, FrameLength>> {};
//static_assert: number of 1s = index of bitset in the array
//Made this an array of size_t instead of bits, because I forgot about duration being a thing that rhythms need to handle.
//Convenience function: diatonic transposition. Move this to another mode without modulating.

template <size_t FrameLength>
class expression : phrase //Should have FrameLength events.
{expression(phrase, rhythm<FrameLength>, key);};
//use rhythm of size_t S
    //construct by passing in a rhythm and a phrase.
    //get a phrase that is in the right number of frames per measure, etc
    //apply a key. This structure should be able to output whatever is needed for a Voice to exactly play notes. So it has a Play method or whatever that's used to do that.
    //Other convenience methods should include modulation (changing the key), etc.
//The game should always play whole expressions, so that nothing is too janky
//This also means that various matchups will have specific FrameLength for all phrases/recurring rhythms, etc.

template <size_t ExpressionFrameLength>
class pattern : vector<expression<ExpressionFrameLength>>
{
    //A pattern is a sequence of phrases.
    //A pattern should contain 0-n expressions. 
    //It should have a method to select and play an expression. 
    //This method should select what to play on the basis of game heuristics.
    //Selectable methods should be able to:
    //  a) maintain internal position and play vector members in a loop
    //  b) jump to specific members if appropriate, including the one that just played
    //  c) point to another pattern and access it instead.
    //Patterns could be used for:
    //  a) rhythms - e.g. there are 4 different drum tracks and 4 different fills, and one is just selected to loop back to itself until some transition, when a fill is selected, etc.
    //  b) continuous melodies - iterate across them until they loop, then go to something else or repeat
    //  c) patterns of patterns. Since patterns can "play" another pattern, they can be used to step up a directed graph of patterns to play, so we can use patterns to time (e.g. "4 cycles" or "end of this vector") transitions among song elements.
    //
    //
    //I think a class along these lines could be the backbone of the song structure... or, more likely, subclasses of it for specific reusable things in songs like drum track or counterpoint line or whatever

};



#endif
