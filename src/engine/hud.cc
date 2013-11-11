#include "hud.h"
#include "interface.h"
HUDElement::HUDElement()
{
	R = 0, G = 0, B = 0, A = 0;
	x = 0, y = 0, w = 1, h = 1;
}

/*
words::words(string t)
{
	text = t;
	HUDElement();
	align = 1;
}

void words::draw(string t)
{
	drawGlyph(text, x, w, y, h, align);
}

template <T> counter
counter::counter(string a);
{
	text = a;
}

template <T> counter
counter::counter(string a, string b);
{
	text = a;
	postText = b;
}

template <T> counter
void counter::draw()
{
	drawGlyph(text + to_string(value) + postText, x, w, y, h, align);
}
*/
