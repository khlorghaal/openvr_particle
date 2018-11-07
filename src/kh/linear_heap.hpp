#pragma once
#include "global.hpp"
#include "vector.hpp"

//maintains order such that indices remain valid
template<typename T>
struct linear_heap{
	vector<T> heap;
	vector<size_t> freed;
	
	size_t alloc(){
		if(!freed.empty())
			return heap[--freed.stop];
		else
			return heap++;
	}
	void free(size_t i){
		freed<<i;
		#ifdef DEBUG
		assert(i>=heap.base && p<heap.stop);//can't check against out of segment pointers
		memset(p,0,SIZEOFT);
		#endif
	}
	T& operator[](size_t i){
		assert(i>=0 && i<heap.size());
		return heap.base[i];
	}
};
