#pragma once

#include "global.hpp"
#include <math.h>

template<typename T>
struct gvec2{
	T x,y;
	gvec2(): x(0),y(0){}
	gvec2(T x): x(x),y(x){};
	gvec2(T x, T y): x(x),y(y){};
	template<typename C>
	gvec2(gvec2<C> c): x((T)c.x),y((T)c.y){};

	inline gvec2<T> operator+(gvec2<T> const a) const { return gvec2<T>(x+a.x, y+a.y); }
	inline gvec2<T> operator-(gvec2<T> const a) const { return gvec2<T>(x-a.x, y-a.y); }
	inline gvec2<T> operator*(gvec2<T> const a) const { return gvec2<T>(x*a.x, y*a.y); }
	inline gvec2<T> operator/(gvec2<T> const a) const { return gvec2<T>(x/a.x, y/a.y); }

	inline gvec2<T>& operator+=(gvec2<T> const a){ x+=a.x, y+=a.y; return *this; }
	inline gvec2<T>& operator-=(gvec2<T> const a){ x-=a.x, y-=a.y; return *this; }
	inline gvec2<T>& operator*=(gvec2<T> const a){ x*=a.x, y*=a.y; return *this; }
	inline gvec2<T>& operator/=(gvec2<T> const a){ x/=a.x, y/=a.y; return *this; }

	template<typename A> inline gvec2<T> operator+(A a) const { return gvec2<T>(x+a,y+a); }
	template<typename A> inline gvec2<T> operator-(A a) const { return gvec2<T>(x-a,y-a); }
	template<typename A> inline gvec2<T> operator*(A a) const { return gvec2<T>(x*a,y*a); }
	template<typename A> inline gvec2<T> operator/(A a) const { return gvec2<T>(x/a,y/a); }

	template<typename A> inline gvec2<T>& operator+=(A a){ x+=a; y+=a; return *this; }
	template<typename A> inline gvec2<T>& operator-=(A a){ x-=a; y-=a; return *this; }
	template<typename A> inline gvec2<T>& operator*=(A a){ x*=a; y*=a; return *this; }
	template<typename A> inline gvec2<T>& operator/=(A a){ x/=a; y/=a; return *this; }

	template<typename A> inline bool operator==(A a){ return a.x==x && a.y==y; }
	template<typename A> inline bool operator!=(A a){ return a.x!=x || a.y!=y; }
};
typedef gvec2<float>    vec2;
typedef gvec2<double>  dvec2;
typedef gvec2<int32>   ivec2;
typedef gvec2<int64>   lvec2;
typedef gvec2<uint32> uivec2;
typedef gvec2<uint64> ulvec2;


template<typename T> inline T len( gvec2<T> const& a){ return sqrt(a.x*a.x + a.y*a.y); }
template<typename T> inline gvec2<T> norm(gvec2<T> const& a){ return a/len(a); }

template<typename T> inline T dot(  gvec2<T> const& a, gvec2<T> const& b){ return a.x*b.x + a.y*b.y; }
//template<typename T> inline gvec2<T> cross(gvec2<T> a, gvec2<T> b){ return ; }

inline float  fract(float x){  return x-(long)x; }
inline double fract(double x){ return x-(long)x; }

template<typename T> inline gvec2<T> sign( gvec2<T> a){ T const& x= a.x, y= a.y; return gvec2<T>( x>0?1:x<0?-1:0, y>0?1:y<0?-1:0); }
template<typename T> inline gvec2<T> fract(gvec2<T> a){ T const& x= a.x, y= a.y; return gvec2<T>( fract(x),fract(y) ); }
template<typename T> inline gvec2<T> floor(gvec2<T> a){ T const& x= a.x, y= a.y; return gvec2<T>( floor(x),floor(y) ); }
template<typename T> inline gvec2<T> ciel( gvec2<T> a){ T const& x= a.x, y= a.y; return gvec2<T>( ciel(x),ciel(y) ); }
template<typename T> inline gvec2<T> sin(  gvec2<T> a){ T const& x= a.x, y= a.y; return gvec2<T>( sin(x),sin(y) ); }
template<typename T> inline gvec2<T> cos(  gvec2<T> a){ T const& x= a.x, y= a.y; return gvec2<T>( cos(x),cos(y) ); }
template<typename T> inline gvec2<T> sqrt( gvec2<T> a){ T const& x= a.x, y= a.y; return gvec2<T>( sqrt(x),sqrt(y) ); }
template<typename T> inline gvec2<T> log(  gvec2<T> a){ T const& x= a.x, y= a.y; return gvec2<T>( log(x),log(y) ); }
template<typename T> inline gvec2<T> log2( gvec2<T> a){ T const& x= a.x, y= a.y; return gvec2<T>( log2(x),log2(y) ); }

template<typename T> inline gvec2<T> pow(gvec2<T> a, T p){ T const& x= a.x, y= a.y; return gvec2<T>( pow(x,p),pow(y,p) ); }
template<typename T> inline gvec2<T> mod(gvec2<T> a, T m){ T const& x= a.x, y= a.y; return gvec2<T>( mod(x,m),mod(y,m) ); }