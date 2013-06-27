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
    c = static_cast<chromatic>(p % 12);
    o = static_cast<octave>(p / 12);
}

size_t pitch::p()
{
    return static_cast<int>(c) + static_cast<int>(o) * 12;
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

pitch key::p(note n, diatonic mode)
{
    return root.add(basis[n.d], n.o);
}

