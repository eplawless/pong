#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>

#include "Engine/Core/Window/Win32Window.h"
#include "Game/game.h"

int WINAPI 
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Win32Window window(L"Pong", 800, 600);
	HWND hwnd = window.GetHandle(); // temporary
	Game game(&window, hwnd);
	game.Run();
	return 0;
}
