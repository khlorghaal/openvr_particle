#include "global.hpp"
#include <iostream>

cout_ cnsl;

cout_& cout_::operator<<(char const* x){
	std::cout<<x;
	return *this;
};
cout_& cout_::operator<<(string x){
	std::cout<<x.cstr;
	return *this;
};
cout_& cout_::operator<<(int x){
	std::cout<<x;
	return *this;
};
cout_& cout_::operator<<(long long x){
	std::cout<<x;
	return *this;
};
cout_& cout_::operator<<(float x){
	std::cout<<x;
	return *this;
};
cout_& cout_::operator<<(double x){
	std::cout<<x;
	return *this;
};

cout_& cout_::operator<<(endl_){
	std::cout<<std::endl;
	return *this;
};
