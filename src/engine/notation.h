#include<array>
#include<bitset>

#ifndef ___notation
#define ___notation

using std::array;
using std::bitset;

enum diatonic {Tonic, Supertonic, Mediant, Subdominant, 
               Dominant, Submediant, Leading, DiatonicDegrees};

enum chromatic {Cr0,Cr1,Cr2,Cr3,Cr4,Cr5,Cr6,Cr7,
                     Cr8,Cr9,Cr10,Cr11,ChromaticOctave};

enum accidental {DoubleFlat=-2, Flat, Neutral, Sharp, DoubleSharp};

typedef array <chromatic, DiatonicDegrees> scale;

struct note
{
	accidental a;
    diatonic d;
    size_t octave; //Octave is between like, 0 and 5
	size_t dynamic; //I think we should use a narrow range of "dynamics" that get applied to overall volume at a different step, e.g. dynamic is probably between like, 0 and 5
	size_t duration; //In frames
};

template <size_t L> class phrase : array<note, L> {};
//methods: mode (output in different mode), modulate (output with root transposed), key (output in different Key), scale, etc
template <size_t S, size_t L> class rhythm : array<bitset<S>, L> {};
//static_assert: number of 1s = index of bitset in the array

template <size_t S, size_t L> class expression : phrase<S> {};
expression(phrase<L>, rhythm<S, L>
//use rhythm of size_t S
    //construct by passing in a rhythm and a phrase.
    //get a phrase that is in the right number of frames per measure, etc

class key {}; //is this useful?

//waaaay too tired to syntax but yeah, i'm thinkin that phrases get played in a Key, which is a Scale + a Mode + a Modulation. Accidentals can still occur on a note-by-note basis if necessary but this should be avoided. 
//a character might have more than one scale. 
//however scales probably still need to be essentially diatonic in basic structure... so that phrases can be translated among keys
//
//
//
//
#endif
