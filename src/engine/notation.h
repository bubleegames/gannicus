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

struct degree
{
    diatonic d;
    accidental a;
    octave o;
};

struct note
{
    pitch p;
    size_t duration;
};

typedef vector<degree> phrase;

class key
{
    scale basis;
    pitch root;
public:
    key(scale, pitch);
    key translate(pitch);
    key transform(scale);
    pitch p(degree);
};

template <size_t FrameLength>
using rhythm = array<array<size_t, FrameLength>, FrameLength>;

template <size_t FrameLength>
class expression : array<note, FrameLength>
{
    expression(phrase p, rhythm<FrameLength> r, key k)
    {   
        int j = 0;
        for (int i = 0; i < FrameLength; i++)
            if (r[i] != 0)
                this[i] = note{k.p(p[j]), r[i]};
    }
};


#endif
