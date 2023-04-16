#pragma once

#include "GraphicClass.h"




class WinClass
{
private:

	friend class GraphicClass;

public:

	WinClass(HINSTANCE hInstance, const wchar_t* tittleName, const wchar_t* windowName, 
									int width, int height, bool FullScreen);
	~WinClass();


	void MainLoop();
	bool InitWindow(HINSTANCE hInstance);

	void FullScreening();

private:
	GraphicClass mRender;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static bool RunRender;

	HWND hWnd;
	WNDCLASSEX wc = {};

	int width, height;
	LPWSTR tittleName, windowName;
	bool fullScreen;
};

