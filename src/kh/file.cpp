#include "file.h"
#include <stdlib.h>
#include <stdio.h>
#include "vector.hpp"


char* file_dump(char const* name){
	FILE* file= fopen(name,"r");
	if(!file)
		return 0;
	
	vector<char> vec(0x800);
	//SEEK_END doesnt always work
	//preallocation is unpossible,
	//but unnecessary
	char buf;
	while(fread(&buf,1,1,file))
		vec<<buf;
	vec<<0;//null terminator
	
	if(ferror(file)){
		fclose(file);
		return 0;
	}

	fclose(file);
	void* ret= vec.base;
	vec.base= 0;//dont free
	return (char*)ret;
}
/*
void load(char const* name, void** dat,long long* len){
	int file= _open(name, 0);
	if(!file)
		goto fail;

	*len= _filelength(file);
	*dat= malloc(*len);
	if(!dat)
		goto fail;


	

	return;
fail:
	*dat= 0;
	*len= 0;
}
*/