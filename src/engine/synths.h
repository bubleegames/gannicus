
class SynthFunctor
{
    int F(size_T) {return F;}
    int operator()(size_t T) {return F(T);}
};

class quadratic : public SynthFunctor
{
    int F(size_T) {return F * F;}
};

