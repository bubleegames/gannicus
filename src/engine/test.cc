#include "notation.h"
#include "scales.h"
#include <iostream>
using std::cout;
using scales::Aeolian;
using scales::Melodic;
int main()
{
    cout << key(Aeolian, pitch(0)).p({Tonic, Neutral, octave1}).p() << "\n";
    cout << key(Aeolian, pitch(0)).p({Submediant, Neutral, octave1}).p() << "\n";
    
    cout << key(Melodic, pitch(0)).p({Submediant, Neutral, octave1}).p() << "\n";
    
}
