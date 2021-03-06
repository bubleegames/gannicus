#include "hud.h"
#include "interface.h"
#include "menu.h"
HUDElement::HUDElement()
{
	R = 0, G = 0, B = 0, A = 0;
	x = 0, y = 0, w = 1, h = 1;
}

words::words()
{
	text = "";
	align = 1;
	R = 0, G = 0, B = 0, A = 0;
	x = 0, y = 0, w = 1, h = 1;
}

words::words(string t)
{
	text = t;
	align = 1;
	R = 0, G = 0, B = 0, A = 0;
	x = 0, y = 0, w = 1, h = 1;
}

words::words(const words &o)
{
	text = o.text;
	align = o.align;
	R = o.R; G = o.G; B = o.B; A = o.A;
	x = o.x; y = o.y; w = o.w; h = o.h;
}

string words::operator()()
{
	return text;
}

string words::operator()(string o)
{
	return text = o;
}

void words::clear()
{
	text.clear();
}

/*
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
