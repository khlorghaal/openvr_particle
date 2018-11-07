#pragma once
#include "string.hpp"
#include <stdlib.h>
#include <string.h>

typedef          char  byte;
typedef unsigned char ubyte;
typedef char      int8;
typedef short     int16;
typedef long      int32;
typedef long long int64;
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned long       uint32;
typedef unsigned long long  uint64;
typedef uint32 ID;

#ifdef _IOSTREAM_
#error
#endif
//hack to greatly reduce compile time
typedef struct{} endl_;
constexpr endl_ endl;
struct cout_{
	cout_& operator<<(char const*);
	cout_& operator<<(string);
	cout_& operator<<(int);
	cout_& operator<<(long long);
	cout_& operator<<(float);
	cout_& operator<<(double);
	cout_& operator<<(endl_);
};
extern cout_ cnsl;

#define error(x) cnsl<<endl<<__FILE__<<":"<<__LINE__ <<endl<<x<<endl


#ifdef NDEBUG
#define assert(b) ((void)0)
#else
#ifndef DEBUG
#define DEBUG
#endif
#define assert(b) if(!(b)){ error("Failed Assertion"); throw; }
#endif

#define lengthof(t) (sizeof(t)/sizeof(*t))

#define SIZEOFT sizeof(T)

//does a swap on raw memory, bypassing any construction or destruction
//only use on plain data
template<typename T>
inline void swap(T& a, T& b){
	void* c= malloc(sizeof(T));//maybe use malloca? but that seems like a bad idea
	memcpy( c,&a,sizeof(T));
	memcpy(&a,&b,sizeof(T));
	memcpy(&b, c,sizeof(T));
	free(c);
}
template<typename T>
inline void copy(T& to, T& from){
	memcpy(&to, &from, SIZEOFT);
}

constexpr uint32 RANDMAX= 0xFFFFFFFF;
inline uint32 rand(uint32 x){
	x= ((x>>16)^x)*0x7FFFFFFFU;
	x= ((x>>16)^x)*0x7FFFFFFFU;
	x=  (x>>16)^x;
	return x;
}
inline float rand(float in){
	return (float)rand(in*RANDMAX) / (float)RANDMAX;
}

template<typename A, typename B=A>
struct pair{
	A a;
	B b;
};
template<typename A, typename B=A, typename C=B>
struct triad{
	A a;
	B b;
	C c;
};