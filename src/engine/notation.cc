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

chromatic key::p(diatonic d)
{
    return basis[d];

}
