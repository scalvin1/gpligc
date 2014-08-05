/*
 * $Id: value.h 3 2014-07-31 09:59:20Z kruegerh $
 * (c) 2002-2013 Hannes Krueger
 * This file is part of the GPLIGC/ogie package
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 * 
 */

#ifndef VALUE_H
#define VALUE_H  "$Id: value.h 3 2014-07-31 09:59:20Z kruegerh $"

#include <iostream>

// ATTENTION
// this template is not suitable for unsigned

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
