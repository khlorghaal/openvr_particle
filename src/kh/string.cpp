#include "string.hpp"
#include <stdlib.h>
#include <string.h>

string::string(){
	cstr= (char*)malloc(1);
	cstr[0]= 0;
	len= 0;
}
string::string(char const* c){
	len= strlen(c);
	cstr= (char*)malloc(len+1);
	memcpy(cstr, c, len+1);
}
string::string(string const& c): string(c.cstr){}
string::~string(){
	free(cstr);
}


void string::operator=(char const* c){
	if(cstr)
		free(cstr);
	len= strlen(c);
	cstr= (char*)malloc(len+1);
	memcpy(cstr, c, len+1);
}
void string::operator=(string const& c){
	operator=(c.cstr);
}


string string::operator+(char const* that){
	string ret;
	size_t lthis= strlen(cstr);
	size_t lthat= strlen(that);
	ret.len= lthis + lthat;
	ret.cstr= (char*)malloc(ret.len+1);

	memcpy(ret.cstr,       cstr, lthis);
	memcpy(ret.cstr+lthis, that, lthat+1);

	return ret;
}
string string::operator+(string const& s){
	return operator+(s.cstr);
}


string& string::operator<<(char const* s){
	*this= *this+s;
	return *this;
}
string& string::operator<<(string const& s){
	return operator<<(s.cstr);
}

bool string::operator==(string const& that){
	if(cstr==that.cstr)
		return true;
	if(len!=that.len)
		return false;

	char const* ia=      cstr;
	char const* ib= that.cstr;
	while(*ia){
		if(*ia!=*ib)
			return false;
		ia++;
		ib++;
	}
	return true;
}

unsigned int hash(char const* s){
	//djb2
	unsigned int x= 5381;
	char c;
	while(c=*s++)
		x= (x<<5)+x+c;
	return x;
}