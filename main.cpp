#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>

#include "Game/game.h"

int WINAPI 
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Game game(TEXT("Pong"));
	game.Run();
	return 0;
}
