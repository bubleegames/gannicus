#ifndef HUD_ELEM
#define HUD_ELEM
#include <SDL/SDL_opengl.h>
#include <iostream>
using std::cout;
class HUDElement{
public:
	HUDElement();
	virtual void draw() = 0;
	double x, y, w, h;
	int R, G, B, A;
};

template <typename T>
class HUDMeter : virtual public HUDElement{
public:
	HUDMeter(T max) {
		maximum = max;
		value = (T)0;
	}

	HUDMeter(const HUDMeter<T>& o) {
		maximum = o.maximum;
		value = o.value;
		R = o.R, G = o.G, B = o.B, A = o.A;
		x = o.x, y = o.y, w = o.w, h = o.h;
	}

	virtual void draw() {
		glColor4f(R, G, B, A);
		glRectf(x, y, x + (double)value / (double)maximum * w, y + h);
	}

	T value;
	T maximum;
};
/*
class words : virtual public HUDElement {
public:
	void words(string);
	virtual void draw();
	string text;
	int align;
};

template <T> counter : virtual public meter, virtual public words
class counter {
public:
	virtual void draw();
	counter(string, string);
	counter(string);
	counter();
	string postText;
};
*/
#endif
