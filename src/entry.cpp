int main_(int argc, char** argv);

#if defined(_GCC)||defined(_CLANG)

int main(int argc, char** argv){
	return main_(argc, argv);
}

#elif _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdio>
#include <cstring>
#include "kh/vector.hpp"

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance, 
	_In_ HINSTANCE hPrevInstance, 
	_In_ LPSTR pCmdLine, 
	_In_ int nCmdShow){
	vector<char*> args;
	
	char* mark= strtok(pCmdLine," ");
	while(!!mark){
		mark= strtok(NULL," ");
		args.push(mark);
	}

	int argc= args.size();
	char** argv= args.base;

	AllocConsole();//VS has no console
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr); 

	return main_(argc, argv);
}

LRESULT CALLBACK WndProc(  
	_In_ HWND   hwnd,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam){
	return 0;
}
#endif