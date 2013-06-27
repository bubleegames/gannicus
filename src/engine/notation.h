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
typedef tuple<chromatic, octave> pitch;
typedef tuple<diatonic, accidental, octave, dynamic, size_t> note;
typedef vector<note> phrase;

class key
{
    scale s;
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
