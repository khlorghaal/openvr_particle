#pragma once
#include <cstring>

typedef unsigned int hash_t;

inline hash_t hash(unsigned int x){
	x= ((x>>16)^x)*0x7FFFFFFFU;
	x= ((x>>16)^x)*0x7FFFFFFFU;
	x=  (x>>16)^x;
	return x;
}
inline hash_t hash(unsigned long long x){
	x= ((x>>32)^x)*0xFFFFFFFFFFFFFFC5U;
	x= ((x>>32)^x)*0xFFFFFFFFFFFFFFC5U;
	x=  (x>>32)^x;
	return x;
}
inline hash_t hash(void* x){ return hash((unsigned long long) x); }

//buckets of fixed depth, expands once a bucket overflows
template<typename K, typename V>
struct hash_map{
	struct slot{
		K k;
		V v;
		bool null;
	};
	#define SLOT_SIZE sizeof(slot)
	#define INIT_LEN 0x20
	#undef GROW_FACTOR
	#define GROW_FACTOR 8
	#define DEPTH 4

	size_t len;//number of buckets
	slot* heap;
	//counting entries is unnecessary, and complicates expansion

	hash_map(size_t init_len);
	hash_map();
	~hash_map();

	void expand();

	//returns null if not contained
	V* operator[](K k);

	//places a key without setting its value
	V* put(K k);
	void remove(K k);

	//removes all elements
	void clear();
};



template<typename K, typename V>
hash_map<K,V>::hash_map(size_t init_len){
	len= init_len;
	size_t memsiz= len*DEPTH*SLOT_SIZE;
	heap= (slot*)malloc(memsiz);
	memset(heap,1,memsiz);
}
template<typename K, typename V>
hash_map<K,V>::hash_map(): hash_map(INIT_LEN){};
template<typename K, typename V>
hash_map<K,V>::~hash_map(){
	if(heap)
		free(heap);
}

template<typename K, typename V> 
void hash_map<K,V>::expand(){
	//copy all non null slots
	slot* entries= (slot*)malloc(len*DEPTH*SLOT_SIZE);
	if(!entries)
		throw;
	size_t entry_iter= 0;
	for(size_t i=0;  i!=len*DEPTH;  i++){
		slot& slot= heap[i];
		if(!slot.null){
			entries[entry_iter++]= slot;
		}
	}

	//reallocate and set nulls
	len= len*GROW_FACTOR;
	size_t size= len*DEPTH*SLOT_SIZE;
	free(heap);
	heap= (slot*)malloc(size);
	if(!heap)
		throw;
	memset(heap,1,size);

	//repopulate
	for(size_t i=0; i!=entry_iter; i++){
		slot& s= entries[i];
		*put(s.k)= s.v;
	}
	free(entries);
}

template<typename K, typename V> 
V* hash_map<K,V>::operator[](K k){
	hash_t h= hash(k);
	size_t i= (h%len)*DEPTH;
	size_t b= 0;
	while(b!=DEPTH){
		slot* at= heap+i+b++;
		if(at->null)//not contained
			return (V*)0;
		if(at->k==k)
			return &at->v;
	}
	//not contained
	return (V*)0;
}

template<typename K, typename V> 
//places a key without setting its value
V* hash_map<K,V>::put(K k){
	hash_t h= hash(k);
	size_t i;
	size_t b;
redo:
	i= (h%len)*DEPTH;
	b= 0;
	while(b!=DEPTH){
		slot& at= heap[i+b++];
		if(at.null){//empty slot found
			at.k= k;
			at.null= false;
			return &at.v;
		}
	}
	//could not fit in bucket
	expand();
	goto redo;//unlikely to happen more than once
}
template<typename K, typename V> 
void hash_map<K,V>::remove(K k){
	hash_t h= hash(k);
	size_t i= (h%len)*DEPTH;
	size_t b= 0;
	while(b!=DEPTH){
		slot& at= heap[i+b++];
		if(at.null)//not contained
			return;
		if(at.k==k){//match
			size_t b2= DEPTH-1;
			//start at end and walk until finding a
			//bucket that is non null. may be self
			slot* swap;
			do 
				swap= heap+i+b2--;
			while(swap->null);

			//there will not be a scenario where
			//b2<b; as at must be non null
			at.k= swap->k;
			at.v= swap->v;
			at.null= swap->null;
			swap->null= true;
			return;
		}
	}
	//not contained
	return;
}

template<typename K, typename V> 
//removes all elements
void hash_map<K,V>::clear(){
	for(slot* i= heap; i!=heap+(len*DEPTH); i++)
		i->null= true;
}