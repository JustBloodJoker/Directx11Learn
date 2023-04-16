#include "stdafx.h"
#include "Utilites.h"
#include "WinClass.h"


int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd) {

	
	WinClass MainHWND(hInstance, L"Hello", L"World", WIDTH, HEIGHT, _WINDOWED);

	return 0;
}


