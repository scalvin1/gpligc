/*
 * (c) 2002-2016 Hannes Krueger
 * This file is part of the GPLIGC/ogie package
 */

#ifndef VALUE_H
#define VALUE_H  1

#include <iostream>

// ATTENTION this template is not suitable for unsigned
template <typename T> class Value
{

	private:
	                T  value;
	                T  max;
	                T  min;



	public:

		Value() {};
		Value (T _val, T _min, T _max);
		T get(void) const {return value;}
		void init(T _val, T _min, T _max);
		void set(T _val) ;
		void inc(T);
		void dec(T);
		void setmax(T _max) { max = _max;}
		T getmax(void) { return max;}
		void setmin(T _min) {min = _min;}
		T getmin(void) {return min;}
};


template <typename T>
Value<T>::Value(T _val, T _min, T _max)
{
	value = _val;
	min = _min;
	max = _max;

	if (value < min) {
		value = min;
		std::cerr << "tried to set value out of range... (min:" << min << ",  tried:" << value << "). " << std::endl;
	}

	if (value > max) {
		value = max;
		std::cerr << "tried to set value out of range... (max:" << max << ",  tried:" << value << "). " << std::endl;
	}


}



template <typename T>
void Value<T>::init(T _val, T _min, T _max)
{
	value = _val;
	min = _min;
	max = _max;

	if (value < min) {
		value = min;
		std::cerr << "tried to set value out of range... (min:" << min << ",  tried:" << value << "). " << std::endl;
	}

	if (value > max) {
		value = max;
		std::cerr << "tried to set value out of range... (max:" << max << ",  tried:" << value << "). " << std::endl;
	}
}

template <typename T>
void Value<T>::inc(T _inc)
{
	value += _inc;
	if (value > max)
		value = max;
}

template <typename T>
void Value<T>::dec(T _dec)
{
	value -= _dec;
	if (value < min)
		value = min;
}

template <typename T>
void Value<T>::set(T _val)
{
	value = _val;
	if (value < min) {
		value = min;
		std::cerr << "tried to set value out of range... (min:" << min << ",  tried:" << value << "). " << std::endl;
	}

	if (value > max) {
		value = max;
		std::cerr << "tried to set value out of range... (max:" << max << ",  tried:" << value << "). " << std::endl;
	}

}

#endif
