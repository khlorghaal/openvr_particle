#pragma once
struct string{
	char* cstr;
	unsigned int len;

	string();
	string(char const*);
	string(string const&);
	~string();

	void operator=(char const*);
	void operator=(string const&);

	string operator+(char const*);
	string operator+(string const&);

	string& operator<<(char const*);
	string& operator<<(string const&);

	inline bool operator!(){ return len==0; };
	bool operator==(string const&);
};

unsigned int hash(char const*);
inline unsigned int hash(string s){ return hash(s.cstr); }