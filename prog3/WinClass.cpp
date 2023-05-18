#pragma once
#include "WinClass.h"

bool WinClass::RunRender = true;

WinClass::WinClass(HINSTANCE hInstance, const wchar_t* tittleName, const wchar_t* windowName, int width, int height, bool FullScreen)
{
    this->fullScreen = FullScreen;
    this->width = width;
    this->height = height;
    this->tittleName = (LPWSTR)tittleName;
    this->windowName = (LPWSTR)windowName;
 
    if (!this->InitWindow(hInstance))
    {
        MessageBox(0, L"Window Initialization - Failed",
            L"Error", MB_OK);
    }

	mRender.SetHWND(hWnd);

	if (!mRender.Init())    
	{
		MessageBox(0, L"Direct3D Initialization - Failed",
			L"Error", MB_OK);
	}

	this->MainLoop();

}

WinClass::~WinClass() = default;

void WinClass::MainLoop()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (RunRender)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			mRender.BeginFrame();
			mRender.Draw();
			mRender.EndFrame();
		}
	}
}

bool WinClass::InitWindow(HINSTANCE hInstance)
{

	FullScreening();

	this->wc.cbSize = sizeof(WNDCLASSEX);
	this->wc.style = CS_HREDRAW | CS_VREDRAW;
	this->wc.lpfnWndProc = WinClass::WndProc;
	this->wc.cbClsExtra = NULL;
	this->wc.cbWndExtra = NULL;
	this->wc.hInstance = hInstance;
	this->wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	this->wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	this->wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	this->wc.lpszMenuName = NULL;
	this->wc.lpszClassName = this->windowName;
	this->wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&this->wc))
	{
		MessageBox(NULL, L"Error registering class",
			L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	this->hWnd = CreateWindowEx(NULL,
		this->windowName,
		this->tittleName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		this->width, this->height,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!this->hWnd)
	{
		MessageBox(NULL, L"Error creating window",
			L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(this->hWnd, 1);
	UpdateWindow(this->hWnd);

	return true;
	
}

void WinClass::FullScreening()
{
    if (this->fullScreen)
    {
        HMONITOR hmon = MonitorFromWindow(hWnd,
            MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfo(hmon, &mi);

        this->width = mi.rcMonitor.right - mi.rcMonitor.left;
        this->height = mi.rcMonitor.bottom - mi.rcMonitor.top;
    }
}

LRESULT WinClass::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (MessageBox(0, L"Are you sure you want to exit?",
				L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				
				RunRender = false;
				DestroyWindow(hwnd);
			}
		}
		return 0;

	case WM_DESTROY: // x button on top right corner of window was pressed
		RunRender = false;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd,
		msg,
		wParam,
		lParam);
}
