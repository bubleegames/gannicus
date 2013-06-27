#include<array>
#include<vector>
#include<map>

#ifndef ___notation
#define ___notation

using std::array;
using std::map;
using std::vector;

template <size_t FrameLength> using rhythm = map<int, <array<size_t, FrameLength>>;

enum diatonic {Tonic, Supertonic, Mediant, Subdominant, Dominant, Submediant, Leading, DiatonicDegrees};
enum chromatic {Cr0,Cr1,Cr2,Cr3,Cr4,Cr5,Cr6,Cr7,Cr8,Cr9,Cr10,Cr11,ChromaticOctave};
enum octave {octave0, octave1, octave2, octave3, octave4};
enum accidental {DoubleFlat=-2, Flat, Neutral, Sharp, DoubleSharp};
enum dynamic {mute, piano, mezzopiano, mezzoforte, forte};
typedef array<chromatic, DiatonicDegrees> scale;
typedef tuple<chromatic, octave> pitch;
typedef tuple<diatonic, accidental, octave, dynamic, size_t> note;
typedef vector<note> phrase;

class key
{
    scale s;
    pitch root;
public:
    key(scale, pitch);
    key modulate(pitch);
    key transform(scale);
    pitch p(note);
    pitch p(note, diatonic); //2nd argument is mode
};

template <size_t FrameLength>
class expression : array<note, FrameLength>
{
    expression(phrase, rhythm<FrameLength>, key);};
//use rhythm of size_t S
    //construct by passing in a rhythm and a phrase.
    //get a phrase that is in the right number of frames per measure, etc
    //apply a key. This structure should be able to output whatever is needed for a Voice to exactly play notes. So it has a Play method or whatever that's used to do that.
    //Other convenience methods should include modulation (changing the key), etc.
//The game should always play whole expressions, so that nothing is too janky
//This also means that various matchups will have specific FrameLength for all phrases/recurring rhythms, etc.

#endif
