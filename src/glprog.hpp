#pragma once
#include "doot/global.hpp"
#include "doot/vector.hpp"
#include "doot/hash_map.hpp"

constexpr int GLSL_VERSION= 450;

struct GLProg{
	int ptr= 0;
	string c=  "";
	string v=  "";
	string tc= "";
	string te= "";
	string g=  "";
	string f=  "";

	hash_map<hash_t, int> umap;
	vector<pair<hash_t, int>> permas;

	GLProg()= default;
	void init(string c);
	void init(string v, string f);
	void init(string v, string g, string f);
	void init(string v, string tc, string te, string f);
	void init(string v, string tc, string te, string g, string f);
	~GLProg();
	GLProg(GLProg const&)=delete;
	GLProg(GLProg const&&)=delete;
	GLProg& operator=(GLProg&)=delete;
	GLProg& operator=(GLProg&&)=delete;
	
	static void reloadAll();
	void reload();

	void initUnis();
	void permaUniform1i(string name, int val);//preserves across refreshing
	static int uniform(string name);//of active

	static GLProg* active;
	void bind();
};
inline int uniform(string name){ return GLProg::uniform(name); }
