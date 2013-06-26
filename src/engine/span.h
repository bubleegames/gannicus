#ifndef SPAN_INCLUDED
#define SPAN_INCLUDED
template <typename T>
struct span {
	T start, end;

	span()
	{
		start = 0;
		end = 0;
	}

	span(T a)
	{
		start = a;
		end = start;
	}

	span(T a, T b)
	{
		start = a;
		end = b;
	}

	bool operator==(const T& o)
	{
		return o >= start && o <= end;
	}

	bool operator!=(const T& o)
	{
		return o < start || o > end;
	}
};
#endif
