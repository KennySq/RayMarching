#include"pch.h"
#include"Engine.h"

HWND windowHandle;

#define WINDOW_CLASS TEXT("Ray Marching (DX12)")
#define WINDOW_NAME WINDOW_CLASS

#define INIT_WIDTH 1280
#define INIT_HEIGHT 720

std::unique_ptr<Engine> engine;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WndProc(HWND handle, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
	if(ImGui_ImplWin32_WndProcHandler(handle, msg, wParam, lParam))
	{
		return true;
	}
	switch (msg)
	{
	case WM_CREATE:
	{

	}
	break;

	case WM_PAINT:
	{
		
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(-1);
		return 0;
	}
	break;
	case WM_QUIT:
	{
	}
	break;
		
	}

	return DefWindowProc(handle, msg, wParam, lParam);
}

void RegisterWindowClass(HINSTANCE hInst)
{
	WNDCLASS wndClass{};

	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = TEXT("Ray Marching (DX12)");
	wndClass.style = CS_VREDRAW | CS_HREDRAW;

	RegisterClass(&wndClass);
	DWORD lastError = GetLastError();
}

int __stdcall WinMain(HINSTANCE handleInst, HINSTANCE previousInstance, LPSTR cmdParam, int nCmdShow)
{
	RegisterWindowClass(handleInst);

	windowHandle = CreateWindow(WINDOW_CLASS, WINDOW_NAME, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, INIT_WIDTH, INIT_HEIGHT, nullptr, nullptr, handleInst, NULL);
	DWORD lastError = GetLastError();
	assert(windowHandle != nullptr);

	ShowWindow(windowHandle, nCmdShow);
	UpdateWindow(windowHandle);

	MSG message = MSG();

	engine = std::make_unique<Engine>(windowHandle, INIT_WIDTH, INIT_HEIGHT);

	engine->Start();

	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			engine->Update();
			engine->Render();
		}
	}

	engine->Release();

	CloseWindow(windowHandle);

	return static_cast<int>(message.wParam);
}