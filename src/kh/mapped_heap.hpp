#pragma once
#include "global.hpp"
#include <cstring>

typedef unsigned long ID;

template<typename T>
struct mapped_heap{
	T* heap; T* begin(){ return heap; };
	T* stop; T* end(){   return stop; };
	T* cap;
	ID* ids;
	uint32* map;
	uint32* mapcap;

	mapped_heap();
	~mapped_heap();

	#undef GROW_FACTOR
	#define GROW_FACTOR 4
	T& alloc(ID);
	void free(ID);

	size_t index(ID);
	T* operator[](ID);
};

template<typename T>
mapped_heap<T>::mapped_heap(){
	constexpr size_t init_cap= 0x20;

	heap=      (T*)malloc(init_cap*sizeof(T));
	 ids=     (ID*)malloc(init_cap*sizeof(ID));
	 map= (uint32*)malloc(init_cap*sizeof(size_t));

	stop= heap;

	   cap= heap+init_cap;
	mapcap=  map+init_cap;

	memset(map, -1, init_cap*sizeof(size_t));
}
template<typename T>
mapped_heap<T>::~mapped_heap(){
	::free(heap);
	::free(ids);
	::free(map);
}

template<typename T>
T& mapped_heap<T>::alloc(ID id){
	if(stop==cap){//expand heap and indices
		size_t siz= cap-heap;
		heap= (T*)realloc(heap,siz*GROW_FACTOR*sizeof(T));
		 ids=(ID*)realloc(ids, siz*GROW_FACTOR*sizeof(size_t));
		 memset(ids+siz, -1, siz*(GROW_FACTOR-1)*sizeof(size_t));
	}
	if(id>=mapcap-map){//expand map
		size_t siz= mapcap-map;
		map= (uint32*)realloc(map, siz*GROW_FACTOR*sizeof(id));
		mapcap= map+(siz*GROW_FACTOR);
		memset(map+siz, -1, siz*(GROW_FACTOR-1)*sizeof(id));
	}

	size_t idx= stop-heap;
	ids[idx]= id;
	map[id]= idx;
	return *stop++;
}

template<typename T>
void mapped_heap<T>::free(ID id){
	size_t idx= index(id);
	if(idx==-1)
		return;
	map[id]= -1;

	//swap with end
	memcpy(heap+idx, --stop,          sizeof(T));
	memcpy( ids+idx, ids+(stop-heap), sizeof(size_t));
}

template<typename T>
size_t mapped_heap<T>::index(ID id){
	if(id<0 || id>=mapcap-map)
		return -1;
	uint32 ret= map[id];
	if(ret==0xffffffff)
		return -1;
	return ret;
}
template<typename T>
T* mapped_heap<T>::operator[](ID id){
	size_t idx= index(id);
	if(idx==-1)
		return (T*)0;
	return heap+idx;
}