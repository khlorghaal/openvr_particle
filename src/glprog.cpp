#include "glprog.hpp"
#include "gl3w.h"
#include "kh/file.h"

void glgetpiv(int iv, int ptr){
	GLint lt;
	glGetProgramiv(ptr, iv, &lt);
	if(!lt){
		GLint ill;
		glGetProgramiv(ptr, GL_INFO_LOG_LENGTH, &ill);
		if(!ill)
			return;

		char* log= (char*)malloc(ill);
		glGetProgramInfoLog(ptr, ill, NULL, log);
		if(log[0]){
			error(log);
			throw;
		}
		free(log);
	}
}



string getShaderSrc(string snam){
	string fnam;
	fnam<<"glsl/"<<snam;
	char* dat= file_dump(fnam.cstr);
	if(!dat){
		error("Cannot open shader src"<<snam);
		throw;
		free(dat);
		return "";
	}

	string ret;
	char verstr[8];
	_itoa(GLSL_VERSION, verstr, 10);
	ret<<"#version "<<verstr<<"\n";
	//TODO includes
	ret<<"#line 1\n";
	ret<<dat;
	free(dat);
	return ret;
}
int doShader(int prog, int type, string fnam){
	int ptr= glCreateShader(type);
	string src= getShaderSrc(fnam);
	const GLchar *const * cstr= &src.cstr;
	glShaderSource(ptr, 1, cstr, 0);
	glCompileShader(ptr);

	GLint lt;
	glGetShaderiv(ptr, GL_COMPILE_STATUS, &lt);
	if(!lt){
		GLint ill;
		glGetShaderiv(ptr, GL_INFO_LOG_LENGTH, &ill);
		if(ill){
			char* log= (char*)malloc(ill);
			glGetShaderInfoLog(ptr, ill, NULL, log);
			if(log[0]){
				error(fnam<<endl<<log);
				throw;
			}
			free(log);
		}
	}

	glAttachShader(prog, ptr);

	return ptr;
}
void detachDelete(int prog, int ptr){
	glDetachShader(prog,ptr);
	glDeleteShader(ptr);
}

void GLProg__init(GLProg& prog, string c, string v, string tc, string te, string g, string f){
	int& ptr= prog.ptr;
	if(ptr)
		glDeleteProgram(ptr);
	ptr= glCreateProgram();

	prog.c=  c;
	prog.v=  v;
	prog.tc= tc;
	prog.te= te;
	prog.g=  g;
	prog.f=  f;
	int c_=  0;
	int v_=  0;
	int tc_= 0;
	int te_= 0;
	int g_=  0;
	int f_=  0;
	if(!!c)
		c_=  doShader(ptr, GL_COMPUTE_SHADER, c);
	if(!!v)
		v_=  doShader(ptr, GL_VERTEX_SHADER, v);
	if(!!tc)
		tc_= doShader(ptr, GL_TESS_CONTROL_SHADER, tc);
	if(!!te)
		te_= doShader(ptr, GL_TESS_EVALUATION_SHADER, te);
	if(!!g)
		g_=  doShader(ptr, GL_GEOMETRY_SHADER, g);
	if(!!f)
		f_=  doShader(ptr, GL_FRAGMENT_SHADER, f);

	glLinkProgram(ptr);
	glgetpiv(GL_LINK_STATUS, ptr);
	glValidateProgram(ptr);
	glgetpiv(GL_VALIDATE_STATUS, ptr);

	if(!!c_)
		detachDelete(ptr, c_);
	if(!!v_)
		detachDelete(ptr, v_);
	if(!!tc_)
		detachDelete(ptr, tc_);
	if(!!te_)
		detachDelete(ptr, te_);
	if(!!g_)
		detachDelete(ptr, g_);
	if(!!f_)
		detachDelete(ptr, f_);

	glUseProgram(ptr);

	prog.initUnis();
}
void GLProg::init(string c){
	GLProg__init(*this, c,"","","","","");
}
void GLProg::init(string v, string f){
	GLProg__init(*this, "", v,"","","", f);
}
void GLProg::init(string v, string g, string f){
	GLProg__init(*this, "", v,"","", g, f);
}
void GLProg::init(string v, string tc, string te, string f){
	GLProg__init(*this, "", v,tc,te,"", f);
}
void GLProg::init(string v, string tc, string te, string g, string f){
	GLProg__init(*this, "", v,tc,te, g, f);
}
GLProg::~GLProg(){
	if(ptr!=0)
		glDeleteProgram(ptr);
	ptr= 0;
	if(active==this)
		active= NULL;
}
void GLProg::reload(){
	GLProg__init(*this, c,v,tc,te,g,f);

	bind();
	for(auto& p : permas){
		glUniform1i(p.a, p.b);
	}
}




GLProg* GLProg::active= NULL;
void GLProg::bind(){
	if(active!=this){
		active= this;
		glUseProgram(ptr);
	}
}



void GLProg::initUnis(){
	umap.clear();
	int count;
	int chbufsiz;
	glGetProgramiv(ptr, GL_ACTIVE_UNIFORMS, &count);
	glGetProgramiv(ptr, GL_ACTIVE_UNIFORM_MAX_LENGTH, &chbufsiz);
	chbufsiz+=1;//null terminator
	char* name= new char[chbufsiz];
	GLenum typ;
	GLint siz;
	for(int i=0; i!=count; i++){
		glGetActiveUniform(ptr, i, chbufsiz, NULL, &siz, &typ, name);
		*umap.put(hash(name))= i;
	}
	free(name);
}
/**Will be preserved if the program gets reloaded.
* Should be used only when initializing*/
void GLProg::permaUniform1i(string name, int val){
	bind();
	if(!umap[hash(name)]){
		error("Uniform "<<name<<" not in program "<<ptr);
		return;
	}
	permas.push({hash(name), val});
	glUniform1i(uniform(name), val);
}

int GLProg::uniform(string name){
	return *active->umap[hash(name)];
}
