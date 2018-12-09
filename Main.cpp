#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>

#include "Engine/Core/Window/Win32Window.h"
#include "Engine/Graphics/Drivers/Direct3D11Renderer.h"
#include "Game/game.h"

int WINAPI 
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Win32Window window(L"Pong", 800, 600);
	Direct3D11Renderer renderer(window);
	Game game(window, renderer);
	game.Run();
	return 0;
}
