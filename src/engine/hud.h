#ifndef HUD_ELEM
#define HUD_ELEM
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
	HUDMeter(T);
	HUDMeter(const HUDMeter<T>&);
	virtual void draw();
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
