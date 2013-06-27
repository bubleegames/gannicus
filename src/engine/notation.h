#include<array>
#include<map>
#include<tuple>
#include<vector>

#ifndef ___notation
#define ___notation

using std::array;
using std::map;
using std::tuple;
using std::vector;

enum diatonic {Tonic, Supertonic, Mediant, Subdominant, Dominant, Submediant, Leading, DiatonicDegrees};
enum chromatic {Cr0,Cr1,Cr2,Cr3,Cr4,Cr5,Cr6,Cr7,Cr8,Cr9,Cr10,Cr11,ChromaticOctave};
enum octave {octave0, octave1, octave2, octave3, octave4};
enum accidental {DoubleFlat=-2, Flat, Neutral, Sharp, DoubleSharp};
enum dynamic {mute, piano, mezzopiano, mezzoforte, forte};

typedef array<chromatic, DiatonicDegrees> scale;

struct pitch
{
    chromatic c;
    octave o;
    pitch();
    pitch(size_t);
    pitch(chromatic, octave);
    size_t p();
    pitch add(size_t);
    pitch add(chromatic, octave);
    pitch subtract(size_t);
    pitch subtract(chromatic, octave);
};

struct note
{
    diatonic d;
    accidental a;
    octave o;
    dynamic v;
    size_t duration;
};

typedef vector<note> phrase;

class key
{
    scale basis;
    pitch root;
public:
    key(scale, pitch);
    key translate(pitch);
    key transform(scale);
    pitch p(note, diatonic=Tonic);
};

template <size_t FrameLength>
using rhythm = array<array<size_t, FrameLength>, FrameLength>;

template <size_t FrameLength>
class expression : array<note, FrameLength>
{
    expression(phrase, rhythm<FrameLength>, key, diatonic=Tonic);
};

#endif
