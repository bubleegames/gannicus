class HUDElement{
public:
	virtual void draw() = 0;
	int x, y;
	int R, G, B, A;
}

template <T> meter : virtual public HUDElement
class meter {
public:
	meter(T);
	virtual void draw();
	T value;
	T maximum;
}

class words : virtual public HUDElement {
public:
	void words(string);
	virtual void draw();
	string text;
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
