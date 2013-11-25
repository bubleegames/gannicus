#ifndef HUD_ELEM
#define HUD_ELEM
#include <SDL/SDL_opengl.h>
#include <iostream>
#include <vector>
#include <string>
using std::vector;
using std::cout;
using std::string;
using std::to_string;
class HUDElement{
public:
	HUDElement();
	virtual void draw() = 0;
	double x, y, w, h;
	float R, G, B, A;
};

template <typename T>
class HUDMeter : virtual public HUDElement{
public:
	string name;
	T value;
	T maximum;

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
		cout << name << ": " << value << '\n';
	}

	virtual T operator=(const T& o) {
		return value = o;
	}
};

class words : virtual public HUDElement {
public:
	words();
	words(string);
	words(const words&);
	virtual void draw();
	virtual void draw(vector<GLuint>);
	virtual string operator()();
	virtual string operator()(string);
	virtual void clear();
	int align;
	string text;
};

template <typename T>
class counter : virtual public HUDMeter<T>, virtual public words{
public:
	string postText;
	counter(string a, string b) {
		text = a;
		postText = b;
		this->value = (T)0;
	}

	counter(string a) {
		text = a;
		postText = "";
		this->value = (T)0;
	}

	counter (T a) {
		text = "";
		postText = "";
		this->value = a;
	}

	counter () {
		text = "";
		postText = "";
		this->value = (T)0;
	}

	void draw(vector<GLuint> glyph) {
		string temp = text;
		text += to_string(this->value) + postText;
		words::draw(glyph);
		text = temp;
	}

	void draw() {
		string temp = text;
		text += to_string(this->value) + postText;
		words::draw();
		text = temp;
	}
};

template <typename T>
class cursor : virtual public counter<T> {
public:
	cursor() { 
		counter<T>(); lock = false; 
	}

	bool operator=(const bool& o){
		return lock = o;
	}

	void draw(vector<GLuint> glyph) {
		if(lock) this->A = .5 + lock*.5;
		counter<T>::draw(glyph);
	}

	bool lock;
};
#endif
