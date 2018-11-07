/* ringbuffer.hpp
 *
 *  Created on: Oct 15, 2015
 *  Author: Khlorghaal
 *	All Rights Reserved.
 *	Do not distribute.
 */

#pragma once

#include <cstring>

template<typename T, size_t const cap>
class ringbuffer{
	T arr[cap];
	size_t p= 0;
public:
	static const size_t capacity= cap;

	ringbuffer(){
		memset(arr,0,sizeof(arr));
	}

	inline void operator<<(T t){ push(t); }
	void push(T t){
		p= (p+1)%cap;
		arr[p]= t;
	}
	T& operator[](size_t i){
		return arr[i%cap];
	}

	T* begin(){ return &arr[0]; }
	T* end(){ return &arr[cap]; }
};