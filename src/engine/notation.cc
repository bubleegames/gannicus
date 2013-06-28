#include "notation.h"

pitch::pitch()
{
    c = Cr0;
    o = octave0;
}

pitch::pitch(chromatic cp, octave oct)
{
    c = cp;
    o = oct;
}

pitch::pitch(size_t p)
{
    c = static_cast<chromatic>(p % ChromaticOctave);
    o = static_cast<octave>(p / ChromaticOctave);
}

size_t pitch::p()
{
    return c + o * ChromaticOctave;
}

pitch pitch::add(size_t delta)
{
    return pitch(p() + delta); 
}

pitch pitch::add(chromatic cp, octave oct)
{
    return pitch(p() + pitch(cp, oct).p());
}

pitch pitch::subtract(size_t delta)
{
    return pitch(p() - delta);
}

pitch pitch::subtract(chromatic cp, octave oct)
{
    return pitch(p() + pitch(cp, oct).p());
}

key::key(scale s, pitch r)
{
    basis = s;
    root = r;
}

key key::translate(pitch r)
{
    return key(basis, r);
}

key key::transform(scale s)
{
    return key(s, root);
}

pitch key::p(degree d)
{
    return root.add(basis[d.d] + d.a + d.o * DiatonicDegrees);
}

expression<size_t FrameLength>::expression(phrase p, rhythm<FrameLength> r, key k)
{
    int j = 0;
    for (int i = 0; i < FrameLength; i++)
        if r[i] != 0
            at(i) = note{k.p(p[j]), r[i]};
}

