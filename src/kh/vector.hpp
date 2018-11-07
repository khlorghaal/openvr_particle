#pragma once
#include "global.hpp"
#include <stdlib.h>

#define SIZT sizeof(T)
template<typename T>
struct vector{
	#ifdef DEBUG
	size_t size_dbg=0;
	#endif
	T* base;
	T* stop;
	T* cap;
	T* begin(){ return base; }
	T* end(){ return stop; }
	#undef GROW_FACTOR
	#define GROW_FACTOR 4
	
	vector(size_t len){
		if(len!=0){
			base= (T*)malloc(len*SIZT); assert(!!base);
			stop= base;
			cap= base+len;
		}
	}
	vector(): vector(8){}

	vector(vector&)= delete;
	vector(vector&&)= delete;
	void operator=(vector const& that){
		this->clear();
		this->realloc(that.capacity());
		for(T* i=that.base; i!=that.stop; i++)
			this->push(*i);
	}

	~vector(){
		clear();
		if(!!base)
			free(base);
	}

	T& operator[](size_t i){ 
		assert(i>=0 && i<size());
		return base[i];
	}

	inline size_t size() const{     return stop-base; }
	inline size_t capacity() const{ return cap-base; }
	inline bool empty() const{ return base==stop; }

	void realloc(size_t l){
		if(l==0)
			l= 1;
		size_t siz= size();
		assert(l*SIZT<0x1000000);
		base= (T*)::realloc(base, l*SIZT);
		assert(!!base);

		cap= base+l;
		stop= base+siz;
		if(cap<stop)
			stop=cap;
	}
	void expand(){
		realloc(capacity()*GROW_FACTOR);
		assert(cap>stop);
	}

	void push(T const& e){
		if(stop==cap)
			expand();
		*(stop++)= e;

		#ifdef DEBUG
		size_dbg++;
		assert(stop<=cap);
		#endif
	}
	vector<T>& operator<<(T const& e){ push(e); return *this; }
	void push(vector<T>& that){
		for(auto& e:that)
			push(e);
	}
	//pushes if element is not contained
	void push_nodup(T const& e){
		size_t f= find(e);
		if(f==-1)
			push(e);
	}

	//allocates an element and returns a pointer to it
	T* operator++(int){
		if(stop==cap)
			expand();
		#ifdef DEBUG
		size_dbg++;
		#endif
		return stop++;
	}
	
	//ret -1 when not found 
	size_t find(T const& e) const{
		for(size_t i=0, s= size(); i!=s; i++){
			if(base[i]==e)
				return i;
		}
		return -1;
	}

	//swaps element with last and shortens
	void remove(size_t i){
		base[i]= *--stop;
		
		#ifdef DEBUG
		assert(i>=0&&i<=size());
		size_dbg--;
		#endif
	}
	//ret true if contained element
	bool remove(T const& e){
		size_t i= find(e);
		if(i==-1)
			return false;
		remove(i);
		return true;
	}

	void clear(){
		stop= base;

		#ifdef DEBUG
		size_dbg= 0;
		#endif
	}

	void print(){
		for(T* i=base; i!=stop; i++)
			cnsl<<*i<<" ";
		cnsl<<endl;
	}
};


template<typename T> void sort_insertion(T* beg, T* end, T* dest){
	assert(false);
	for(T* i= beg; i!=end; i++){

	}
}